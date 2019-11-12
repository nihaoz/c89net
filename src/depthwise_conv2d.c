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
 *			int sx, int sy, int p, void *filter, int filter_width);
 */

/* #include "global_function_config.h" */

extern void (*_conv_2d_float32)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

/* _conv_2d_handler for current processing datatype */
static void (*_conv_2d_handler)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

feature_map_t *depthwise_conv2d(feature_map_t *inp,
		cnn_para_t *kernel, int s, int p, const char *name)
{
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
	}
	/* Add padding */
	char padding_name[PADDING_NAME_BUF_LEN];
	sprintf(padding_name, "%s%s", name, CONV_PAD_NAME_SURFFIX);
	feature_map_t *inp_pad = pad_surround(inp, p, padding_name);
	if (!inp_pad)
		return NULL;
#ifdef ENABLE_MEMMGR
	feature_map_t *oup =
		(feature_map_t*)
			memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
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
		oup->xsize = conv_2d_size_calc(inp->xsize,
					kernel->xsize, s, p);
		oup->ysize = conv_2d_size_calc(inp->ysize,
					kernel->ysize, s, p);
		oup->zsize = kernel->zsize;
		oup->data  = list_new_static(oup->zsize,
				sizeof(float32) * oup->xsize * oup->ysize);
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
	int o_ch_mem_size = oup_ch_size * sizeof(float32);
	int k_ch_mem_size = kernel->xsize * kernel->ysize * sizeof(float32);
	int i;
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < kernel->wsize; ++i)
	{
		_conv_2d_handler(
			(inp_pad->data->mem + i * p_ch_mem_size),
				(oup->data->mem + i * o_ch_mem_size),
				inp_pad->xsize, inp_pad->ysize, s, s, p, 
			(kernel->data->mem + i * k_ch_mem_size),
		kernel->xsize);
	}
#ifndef ENABLE_MEMMGR
	free_feature_map(inp_pad);
#endif
	return oup;
}
