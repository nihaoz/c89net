#ifndef _POOL_H_
#define _POOL_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

/*
 * For testing only, using max_pool2_2 before implement
 * generic max_pool
 * feature_map_t *max_pool(int sx, int y, int padding);
 */
feature_map_t *max_pool2_2(feature_map_t *l, const char *name);

void _max_pool2_2(float32 *inp, int x, int y, float32 *oup);

#ifdef __cplusplus
	}
#endif

#endif /* _POOL_H_ */