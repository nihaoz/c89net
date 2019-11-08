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
#include "spatial_conv2d.h"

/*
 * Ref: global_function_config.h
 * void (*_conv_2d)(void *inp, void *oup, int x, int y,
 *			int sx, int sy, int p, void *filter, int filter_width);
 */

/* #include "global_function_config.h" */

extern void (*_conv_2d_float32)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

/* _conv_2d_handler for current processing datatype */
static void (*_conv_2d_handler)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

feature_map_t *spatial_conv2d(feature_map_t *inp, cnn_para_t *kernel,
						cnn_para_t *bias, int s, int p, const char *name)
{
	/* Parameter check */
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
	}
	/* Add padding */
	char padding_name[PADDING_NAME_BUF_LEN];
	sprintf(padding_name, "%s%s", name, CONV_PAD_NAME_SURFFIX);
	feature_map_t *inp_pad = pad_surround(inp, p, padding_name);
	if (!inp_pad)
		return NULL;
#ifdef ENABLE_MEMMGR
	feature_map_t *oup = (feature_map_t*)memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *oup = NULL;
#endif
	if (!oup) {
		oup = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!oup) {
#ifndef ENABLE_MEMMGR
			free_feature_map(inp_pad);
#endif
			return NULL;
		}
		oup->datatype = inp->datatype;
		oup->xsize = conv_2d_size_calc(inp->xsize, kernel->xsize, s, p);
		oup->ysize = conv_2d_size_calc(inp->ysize, kernel->ysize, s, p);
		oup->zsize = kernel->wsize;
		oup->data  = list_new_static(kernel->wsize, sizeof(float32) * oup->xsize * oup->ysize);
		if (!oup->data) {
			free(oup);
#ifndef ENABLE_MEMMGR
			free_feature_map(inp_pad);
#endif
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	int oup_ch_size = oup->xsize * oup->ysize;
	int p_ch_mem_size = inp_pad->xsize * inp_pad->ysize * sizeof(float32);
	int o_ch_mem_size = oup->xsize * oup->ysize * sizeof(float32);
	int k_mem_size    = kernel->xsize * kernel->ysize * kernel->zsize * sizeof(float32);
	int k_ch_mem_size = kernel->xsize * kernel->ysize * sizeof(float32);
	int i, j, k;
	int num_omp_threads = 1;
#ifdef ENABLE_OPENMP
	num_omp_threads = omp_get_max_threads();
#endif
	float32 *omp_out_buf = 
		(float32*)malloc(sizeof(float32) * oup_ch_size * num_omp_threads);
	if (!omp_out_buf) {
#ifndef ENABLE_MEMMGR
		free_feature_map(oup);
		free_feature_map(inp_pad);
#endif
		return NULL;
	}
 /* reset memory each time when reuse exist feature map */
#ifdef ENABLE_MEMMGR
	memset(oup->data->mem, 0, oup->data->length);
#endif
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i, j, k)
#endif
	for (i = 0; i < kernel->wsize; ++i)
	{
		for (j = 0; j < kernel->zsize; ++j)
		{
#ifdef ENABLE_OPENMP
			_conv_2d_handler((inp_pad->data->mem + j * p_ch_mem_size),
								omp_out_buf + omp_get_thread_num() * oup_ch_size,
							inp_pad->xsize, inp_pad->ysize, s, s, p, 
					(kernel->data->mem + (i * k_mem_size) + j * k_ch_mem_size),
				kernel->xsize);
#else
			_conv_2d_handler((inp_pad->data->mem + j * p_ch_mem_size),
							omp_out_buf, inp_pad->xsize, inp_pad->ysize, s, s, p, 
					(kernel->data->mem + (i * k_mem_size) + j * k_ch_mem_size),
				kernel->xsize);

#endif
			for (k = 0; k < oup_ch_size; ++k)
			{
#ifdef ENABLE_OPENMP
				*(((float32*)(oup->data->mem + i * o_ch_mem_size)) + k) +=
						(omp_out_buf + omp_get_thread_num() * oup_ch_size)[k];
#else
				*(((float32*)(oup->data->mem + i * o_ch_mem_size)) + k) += omp_out_buf[k];
#endif
			}
		}
	}
	free(omp_out_buf);
	if (!bias){
#ifndef ENABLE_MEMMGR
		free_feature_map(inp_pad);
#endif
		return oup;
	} else {
		float32 *p_bias = bias_from_cnn_parameters(bias);
		for (i = 0; i < kernel->wsize; ++i)
		{
			for (k = 0; k < oup_ch_size; ++k)
			{
				*(((float32*)(oup->data->mem + i * o_ch_mem_size)) + k) += p_bias[i];
			}
		}
	}
#ifndef ENABLE_MEMMGR
	free_feature_map(inp_pad);
#endif
	return oup;
}
