/* Depthwise Separable Convolution */
#ifndef _DEPTHWISE_CONV_H_
#define _DEPTHWISE_CONV_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "conv2d.h"
#include "data_layer.h"

feature_map_t *depthwise_conv2d(feature_map_t *inp, cnn_para_t *kernel,
			int s, int p, int poffset, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _DEPTHWISE_CONV_H_ */