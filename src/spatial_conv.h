#ifndef _SPATIAL_CONV_H_
#define _SPATIAL_CONV_H_
/* Spatial Convolution */

#ifdef __cplusplus
	extern "C" {
#endif

#include "conv2d.h"
#include "data_layer.h"

data_layer *spatial_conv(data_layer *inp, para_layer *kernel,
							para_layer *bias, int s, int p);

#ifdef __cplusplus
	}
#endif

#endif /* _SPATIAL_CONV_H_ */