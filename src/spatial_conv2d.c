#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_OPENMP
	#include <omp.h>
#endif
#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "array_ops.h"
#include "debug_log.h"
#include "pad.h"
#include "fmap_ops.h"
#include "spatial_conv2d.h"

/*
 * Ref: global_function_config.h
 * void (*_conv_2d)(void *inp, void *oup, int x, int y,
 *	int sx, int sy, int p, void *filter, int filter_width);
 */

/* #include "global_function_config.h" */

extern void (*_conv_2d_float32)(void *inp, void *oup,
		int x, int y, int oup_x, int oup_y, int sx, int sy,
	void *filter, int fw);

/* _conv_2d_handler for current processing datatype */
static void (*_conv_2d_handler)(void *inp, void *oup,
		int x, int y, int oup_x, int oup_y, int sx, int sy,
	void *filter, int fw);

feature_map_t *spatial_conv2d(feature_map_t *inp,
			cnn_para_t *kernel, cnn_para_t *bias,
			int s, int p, int poffset, const char *name)
{
	char padding_name[PADDING_NAME_BUF_LEN];
	feature_map_t *inp_pad = NULL;
	feature_map_t *oup     = NULL;
	int o_ch_size, p_ch_mem_size, o_ch_mem_size, k_ch_mem_size,
		k_mem_size, num_omp_threads, i, j;
	byte *omp_out_buf = NULL;
	/* Parameter check */
	if (inp->zsize != kernel->zsize) {
		QUICK_LOG_BAD_ARG((inp->zsize != kernel->zsize));
		return NULL;
	}
	/* Datatype check */
	if (inp->datatype != kernel->datatype) {
		QUICK_LOG_ERR_DATATYPE((inp->datatype != kernel->datatype));
		return NULL;
	}
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_conv_2d_handler = _conv_2d_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
	/* Add padding */
	if (p) {
		sprintf(padding_name, "%s%s", name, CONV_PAD_NAME_SURFFIX);
		inp_pad = pad_surround(inp, p, poffset, padding_name);
	} else {
		inp_pad = inp;
	}
	if (!inp_pad)
		return NULL;
#ifdef ENABLE_MEMMGR
	oup = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!oup) {
		oup = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!oup) {
#ifndef ENABLE_MEMMGR
			if (p)
				free_feature_map(inp_pad);
#endif
			return NULL;
		}
		oup->datatype = inp->datatype;
		oup->xsize    = conv_2d_size_calc(inp->xsize,
					kernel->xsize, s, p);
		oup->ysize    = conv_2d_size_calc(inp->ysize,
					kernel->ysize, s, p);
		o_ch_size     = oup->xsize * oup->ysize;
		o_ch_mem_size = o_ch_size * sizeof_datatype(oup->datatype);
		oup->zsize    = kernel->wsize;
		oup->data     = list_new_static(oup->zsize, o_ch_mem_size);
		if (!oup->data) {
			free(oup);
#ifndef ENABLE_MEMMGR
			if (p)
				free_feature_map(inp_pad);
#endif
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	o_ch_size     = oup->xsize * oup->ysize;
	o_ch_mem_size = o_ch_size * sizeof_datatype(oup->datatype);
	p_ch_mem_size = inp_pad->xsize * inp_pad->ysize *
					sizeof_datatype(inp->datatype);
	k_ch_mem_size = kernel->xsize * kernel->ysize *
					sizeof_datatype(kernel->datatype);
	k_mem_size    = k_ch_mem_size * kernel->zsize;
	num_omp_threads = 1;
#ifdef ENABLE_OPENMP
	num_omp_threads = omp_get_max_threads();
#endif
	omp_out_buf = 
		(byte*)malloc(o_ch_mem_size * num_omp_threads);
	if (!omp_out_buf) {
#ifndef ENABLE_MEMMGR
		free_feature_map(oup);
		if (p)
			free_feature_map(inp_pad);
#endif
		return NULL;
	}
 /* reset memory each time when reuse exist feature map */
#ifdef ENABLE_MEMMGR
	memset(oup->data->mem, 0, oup->data->length);
#endif
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i, j)
#endif
	for (i = 0; i < kernel->wsize; ++i)
	{
		for (j = 0; j < kernel->zsize; ++j)
		{
#ifdef ENABLE_OPENMP
		_conv_2d_handler(
			(inp_pad->data->mem + j * p_ch_mem_size),
			omp_out_buf + omp_get_thread_num() * o_ch_mem_size,
				inp_pad->xsize, inp_pad->ysize,
				oup->xsize, oup->ysize, s, s, 
			(kernel->data->mem + (k_mem_size * i) + 
			k_ch_mem_size * j), kernel->xsize);
		array_ops_add(oup->data->mem + o_ch_mem_size * i,
			omp_out_buf + omp_get_thread_num() * o_ch_mem_size,
		o_ch_size, oup->datatype);
#else
		_conv_2d_handler((inp_pad->data->mem + p_ch_mem_size * j),
			omp_out_buf, inp_pad->xsize, inp_pad->ysize,
					oup->xsize, oup->ysize, s, s,
			(kernel->data->mem + (k_mem_size * i) + 
			k_ch_mem_size * j), kernel->xsize);
		array_ops_add(oup->data->mem + o_ch_mem_size * i,
			omp_out_buf, o_ch_size, oup->datatype);

#endif
		}
	}
	free(omp_out_buf);
	if (!bias){
#ifndef ENABLE_MEMMGR
		if (p)
			free_feature_map(inp_pad);
#endif
		return oup;
	} else {
		oup = feature_map_bias(oup, bias);
	}
#ifndef ENABLE_MEMMGR
	if (p)
		free_feature_map(inp_pad);
#endif
	return oup;
}
