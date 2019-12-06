#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_OPENMP
	#include <omp.h>
#endif
#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "pad.h"
#include "depthwise_conv2d.h"

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

feature_map_t *depthwise_conv2d(feature_map_t *inp, cnn_para_t *kernel,
			int s, int p, int poffset, const char *name)
{
	char padding_name[PADDING_NAME_BUF_LEN];
	feature_map_t *inp_pad = NULL;
	feature_map_t *oup     = NULL;
	int o_ch_size, o_ch_mem_size, p_ch_mem_size, k_ch_mem_size, i;
	/* Parameter check, kernel - wsize should be 1 */
	if (inp->zsize != kernel->zsize) {
		QUICK_LOG_ERR_DATATYPE((inp->zsize != kernel->zsize));
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
			return NULL;
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
			QUICK_LOG_ERR_MEM_ALLOC(oup);
			return NULL;
		}
		oup->datatype = inp->datatype;
		oup->xsize    = conv_2d_size_calc(inp->xsize,
					kernel->xsize, s, p);
		oup->ysize    = conv_2d_size_calc(inp->ysize,
					kernel->ysize, s, p);
		oup->zsize    = kernel->zsize;
		o_ch_size     = oup->xsize * oup->ysize;
		o_ch_mem_size = o_ch_size * sizeof_datatype(oup->datatype);
		oup->data     = list_new_static(oup->zsize, o_ch_mem_size);
		if (!oup->data) {
			free(oup);
#ifndef ENABLE_MEMMGR
			if (p)
				free_feature_map(inp_pad);
#endif
			QUICK_LOG_ERR_MEM_ALLOC(oup->data);
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
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < kernel->zsize; ++i)
	{
		_conv_2d_handler(
			(inp_pad->data->mem + p_ch_mem_size * i),
				(oup->data->mem + o_ch_mem_size * i),
					inp_pad->xsize, inp_pad->ysize,
				oup->xsize, oup->ysize, s, s, 
			(kernel->data->mem + k_ch_mem_size * i),
		kernel->xsize);
	}
#ifndef ENABLE_MEMMGR
	if (p)
		free_feature_map(inp_pad);
#endif
	return oup;
}
