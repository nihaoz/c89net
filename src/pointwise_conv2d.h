/*
 * Efficient pointwise conv(equal to spatial conv by 1 * 1 kernel)
 * For low-end CPU...
 */
#ifndef _POINTWISE_CONV2D_H_
#define _POINTWISE_CONV2D_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

feature_map_t *pointwise_conv2d(feature_map_t *inp, cnn_para_t *kernel,
					cnn_para_t *bias, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _POINTWISE_CONV2D_H_ */