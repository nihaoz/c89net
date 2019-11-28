#include <stdio.h>
#include <stdlib.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "array_ops.h"
#include "data_util.h"
#include "pool.h"
#include "list.h"

/* #include "global_function_config.h" */
extern void (*_max_pool2_2_float32)(void *inp,
			int x, int y, void *oup);

static void (*_max_pool2_2_handler)(void *inp,
			int x, int y, void *oup);

static float32 _max(float32 a, float32 b)
{
	return  a > b ? a : b;
}

feature_map_t *max_pool2_2(feature_map_t *l, const char *name)
{
	int i, i_ch_size, i_ch_mem_size, o_ch_size, o_ch_mem_size;
	feature_map_t *pool = NULL;
	switch (l->datatype) {
		case DATATYPE_FLOAT32:
			_max_pool2_2_handler = _max_pool2_2_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(l->datatype);
	}
#ifdef ENABLE_MEMMGR
	pool = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pool) {
		pool = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pool)
			return NULL;
		pool->datatype = l->datatype;
		pool->xsize = l->xsize >> 1;
		pool->ysize = l->ysize >> 1;
		pool->zsize = l->zsize;
		pool->data  = list_new_static(pool->zsize,
				pool->xsize * pool->ysize * 
				sizeof_datatype(pool->datatype));
		if (!pool->data) {
			free(pool);
			return NULL;
		}
		list_set_name(pool->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, pool);
#endif
	}
	i_ch_size     = l->xsize * l->ysize;
	i_ch_mem_size = i_ch_size * sizeof_datatype(l->datatype);
	o_ch_size     = pool->xsize * pool->ysize;
	o_ch_mem_size = o_ch_size * sizeof_datatype(pool->datatype);
	for (i = 0; i < l->zsize; ++i)
	{
		_max_pool2_2_handler(l->data->mem + i_ch_mem_size * i,
							l->xsize, l->ysize,
				pool->data->mem + o_ch_mem_size * i);
	}
	return pool;
}

feature_map_t *channel_avg(feature_map_t *l, const char *name)
{
	int i_ch_size, i_ch_mem_size, i;
	feature_map_t *pool = NULL;
#ifdef ENABLE_MEMMGR
	pool = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pool) {
		pool = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pool)
			return NULL;
		pool->datatype = l->datatype;
		pool->xsize    = 1;
		pool->ysize    = 1;
		pool->zsize    = l->zsize;
		pool->data     = list_new_static(pool->zsize,
			sizeof_datatype(pool->datatype));
		if (!pool->data) {
			free(pool);
			return NULL;
		}
		list_set_name(pool->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, pool);
#endif
	}
	i_ch_size     = l->xsize * l->ysize;
	i_ch_mem_size = i_ch_size * sizeof_datatype(l->datatype);
	for (i = 0; i < l->zsize; ++i)
	{
		array_mean(l->data->mem + i_ch_mem_size * i, i_ch_size,
			pool->data->mem + sizeof_datatype(pool->datatype) * i,
			pool->datatype);
	}
	return pool;
}

void naive_max_pool2_2_float32(float32 *inp, int x, int y, float32 *oup)
{
	int xc = x >> 1;
	int yc = y >> 1;
	int i, j;
	for (i = 0; i < yc; ++i) 
	{
		for (j = 0; j < xc; ++j)
		{
			oup[i * xc + j] = _max(_max(inp[(i * xc * 4) + (j * 2)],
					inp[(i * xc * 4) + (j * 2) + 1]),
				_max(inp[((i * 2) + 1) * (xc * 2) + j * 2],
			inp[((i * 2) + 1) * (xc * 2) + (j * 2) + 1]));
		}
	}
	return;
}
