#ifndef _STD_CONV2D_H_
#define _STD_CONV2D_H_

/* Standrad 2-D conv */

#ifdef __cplusplus
	extern "C" {
#endif

#include "conv2d.h"
#include "data_layer.h"

data_layer *std_conv2d(data_layer *inp, para_layer *kernel,
							para_layer *bias, int s, int p);

#ifdef __cplusplus
	}
#endif

#endif /* _STD_CONV2D_H_ */