/* Spatial Convolution */
#ifndef _SPATIAL_CONV_H_
#define _SPATIAL_CONV_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "conv2d.h"
#include "data_layer.h"

#define SP_CONV_PAD_SURFFIX "_pad"

feature_map_t *spatial_conv(feature_map_t *inp, cnn_para_t *kernel,
						cnn_para_t *bias, int s, int p, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _SPATIAL_CONV_H_ */