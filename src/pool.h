#ifndef _POOL_H_
#define _POOL_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

/*
 * For testing only, using max_pool2_2 before implement
 * generic max_pool
 * data_layer *max_pool(int sx, int y, int padding);
 */
data_layer *max_pool2_2(data_layer *l);

void _max_pool2_2(float32 *inp, int x, int y, float32 *oup);

#ifdef __cplusplus
	}
#endif

#endif /* _POOL_H_ */