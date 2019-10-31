#include <stdlib.h>

#include "data_util.h"
#include "pool.h"
#include "list.h"

static float32 max(float32 a, float32 b)
{
	return  a > b ? a : b;
}

feature_map_t *max_pool2_2(feature_map_t *l)
{
	feature_map_t *pool = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!pool)
		return NULL;
	pool->datatype = l->datatype;
	pool->xsize = l->xsize >> 1;
	pool->ysize = l->ysize >> 1;
	pool->zsize = l->zsize;
	pool->data  = list_new_static(pool->zsize,
						sizeof(float32) * pool->xsize * pool->ysize);
	if (!pool->data) {
		free(pool);
		return NULL;
	}
	int i, in_ch_size = l->xsize * l->ysize;
	int out_ch_size   = pool->xsize * pool->ysize;
	for (i = 0; i < l->zsize; ++i)
	{
		_max_pool2_2(((float32*)l->data->mem) + i * in_ch_size,
			l->xsize, l->ysize, ((float32*)pool->data->mem + i * out_ch_size));
	}
	return pool;
}

void _max_pool2_2(float32 *inp, int x, int y, float32 *oup)
{
	int xc = x >> 1;
	int yc = y >> 1;
	int i, j;
	for (i = 0; i < yc; ++i) 
	{
		for (j = 0; j < xc; ++j)
		{
			oup[i * xc + j] = max(max(inp[(i * xc * 4) + (j * 2)],
									inp[(i * xc * 4) + (j * 2) + 1]),
						max(inp[((i * 2) + 1) * (xc * 2) + j * 2],
				inp[((i * 2) + 1) * (xc * 2) + (j * 2) + 1]));
		}
	}
	return;
}
