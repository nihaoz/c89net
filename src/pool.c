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


extern void (*_max_pool_generic)(void *inp,
	int x, int y, int s, void *oup, int dt);

/*
 * static void (*_max_pool_handler)(void *inp,
 *	int x, int y, int s, void *oup, int dt);
 */

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
			return NULL;
	}
#ifdef ENABLE_MEMMGR
	pool = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pool) {
		pool = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pool) {
			QUICK_LOG_ERR_MEM_ALLOC(pool);
			return NULL;
		}
		pool->datatype = l->datatype;
		pool->xsize = l->xsize >> 1;
		pool->ysize = l->ysize >> 1;
		pool->zsize = l->zsize;
		pool->data  = list_new_static(pool->zsize,
				pool->xsize * pool->ysize * 
				sizeof_datatype(pool->datatype));
		if (!pool->data) {
			free(pool);
			QUICK_LOG_ERR_MEM_ALLOC(pool->data);
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

feature_map_t *max_pool(feature_map_t *l, int s, const char *name)
{
	int i, i_ch_size, i_ch_mem_size, o_ch_size, o_ch_mem_size;
	feature_map_t *pool = NULL;
#ifdef ENABLE_MEMMGR
	pool = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pool) {
		pool = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pool) {
			QUICK_LOG_ERR_MEM_ALLOC(pool);
			return NULL;
		}
		pool->datatype = l->datatype;
		pool->xsize = l->xsize / s;
		pool->ysize = l->ysize / s;
		pool->zsize = l->zsize;
		pool->data  = list_new_static(pool->zsize,
				pool->xsize * pool->ysize * 
				sizeof_datatype(pool->datatype));
		if (!pool->data) {
			free(pool);
			QUICK_LOG_ERR_MEM_ALLOC(pool->data);
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
		_max_pool_generic(l->data->mem + i_ch_mem_size * i,
						l->xsize, l->ysize, s,
				pool->data->mem + o_ch_mem_size * i,
				l->datatype);
	}
	return pool;
}

feature_map_t *global_avg_pool(feature_map_t *l, const char *name)
{
	int i_ch_size, i_ch_mem_size, i;
	feature_map_t *pool = NULL;
#ifdef ENABLE_MEMMGR
	pool = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pool) {
		pool = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pool) {
			QUICK_LOG_ERR_MEM_ALLOC(pool);
			return NULL;
		}
		pool->datatype = l->datatype;
		pool->xsize    = 1;
		pool->ysize    = 1;
		pool->zsize    = l->zsize;
		pool->data     = list_new_static(pool->zsize,
			sizeof_datatype(pool->datatype));
		if (!pool->data) {
			free(pool);
			QUICK_LOG_ERR_MEM_ALLOC(pool->data);
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

#define MAX_POOL_SUB_PROCESS(datatype) \
for (i = 0; i < o_y; ++i)                                                  \
{                                                                          \
	for (j = 0; j < o_x; ++j)                                          \
	{                                                                  \
		*(datatype*)v_max = *((datatype*)inp + s * i * x + s * j); \
		for (k = 0; k < s; ++k)                                    \
		{                                                          \
			for (l = 0; l < s; ++l)                            \
			{                                                  \
				curr = (datatype*)inp +                    \
					(s * i + k) * x + j * s + l;       \
				*(datatype*)v_max = *((datatype*)curr) >   \
					*(datatype*)v_max ?                \
				*((datatype*)curr) : *(datatype*)v_max;    \
			}                                                  \
		}                                                          \
		*((datatype*)oup + i * o_x + j) = *(datatype*)v_max;       \
	}                                                                  \
}

void naive_max_pool_generic(void *inp,
	int x, int y, int s, void *oup, int dt)
{
	int o_x = x / s;
	int o_y = y / s;
	int i, j, k, l;
	void *curr;
	void *v_max = malloc(sizeof_datatype(dt));
	if (!v_max) {
		QUICK_LOG_ERR_MEM_ALLOC(v_max);
		return;
	}
	switch (dt) {
		case DATATYPE_FLOAT32:
			MAX_POOL_SUB_PROCESS(float32);
			break;
		case DATATYPE_FLOAT64:
			MAX_POOL_SUB_PROCESS(float64);
			break;
		case DATATYPE_INT16:
			MAX_POOL_SUB_PROCESS(int16);
			break;
		case DATATYPE_INT32:
			MAX_POOL_SUB_PROCESS(int32);
			break;
		case DATATYPE_INT64:
			MAX_POOL_SUB_PROCESS(int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(datatype);
			break;
	}
	free(v_max);
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
}
