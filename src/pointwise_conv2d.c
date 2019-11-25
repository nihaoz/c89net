#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_OPENMP
	#include <omp.h>
#endif
#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "array_ops.h"
#include "fmap_ops.h"
#include "debug_log.h"
#include "pointwise_conv2d.h"

feature_map_t *pointwise_conv2d(feature_map_t *inp, cnn_para_t *kernel,
					cnn_para_t *bias, const char *name)
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
#ifdef ENABLE_MEMMGR
	feature_map_t *oup =
		(feature_map_t*)
			memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *oup = NULL;
#endif
	if (!oup) {
		oup = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!oup)
			return NULL;
		oup->datatype = inp->datatype;
		oup->xsize = inp->xsize;
		oup->ysize = inp->ysize;
		oup->zsize = kernel->wsize;
		oup->data  = list_new_static(kernel->wsize,
				sizeof_datatype(oup->datatype) *
						oup->xsize * oup->ysize);
		if (!oup->data) {
			free(oup);
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	int i_ch_size = inp->xsize * inp->ysize;
	int i_ch_mem_size = i_ch_size * sizeof_datatype(inp->datatype);
	int o_ch_size = oup->xsize * oup->ysize;
	int o_ch_mem_size = o_ch_size * sizeof_datatype(oup->datatype);
	int k_ch_mem_size = sizeof_datatype(kernel->datatype);
	int k_mem_size = k_ch_mem_size * kernel->zsize;
	int i, j;
	int num_omp_threads = 1;
#ifdef ENABLE_OPENMP
	num_omp_threads = omp_get_max_threads();
#endif
	byte *omp_out_buf = 
		(byte*)malloc(o_ch_mem_size * num_omp_threads);
	if (!omp_out_buf) {
#ifndef ENABLE_MEMMGR
		free_feature_map(oup);
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
		mul_to_array(inp->data->mem + i_ch_mem_size * j,
			omp_out_buf + omp_get_thread_num() * o_ch_mem_size,
			i_ch_size, kernel->data->mem + k_mem_size * i +
				k_ch_mem_size * j, kernel->datatype);
		array_ops_add(oup->data->mem + o_ch_mem_size * i,
			omp_out_buf + omp_get_thread_num() * o_ch_mem_size,
			o_ch_size, oup->datatype);
#else
		mul_to_array(inp->data->mem + i_ch_mem_size * j,
						omp_out_buf, i_ch_size,
				kernel->data->mem + k_mem_size * i +
				k_ch_mem_size * j, kernel->datatype);
		array_ops_add(oup->data->mem + o_ch_mem_size * i,
			omp_out_buf, o_ch_size, oup->datatype);

#endif
		}
	}
	free(omp_out_buf);
	if (!bias){
		return oup;
	} else {
		oup = feature_map_bias(oup, bias);
	}
	return oup;
}