#ifndef _CONV2D_H_
#define _CONV2D_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_util.h"

/*
 * typedef float32 conv_input_t;
 * typedef float32 conv_filter_t;
 */

channel_t *conv2d_filter(float32 *data, int size);

channel_t *channel_conv2d(channel_t *inp, channel_t *filter, int s, int p);

/*
 * Calc output size after conv2d
 * inp: input size
 * k  : kernel size
 * s  : stride
 * p  : padding
 */
int conv_2d_size_calc(int inp, int k, int s, int p);

/*
 * Pre-allocate memory for output by conv2d
 * k : kernel size
 * s : stride
 * p : padding
 */
channel_t *conv_2d_malloc(channel_t *ch, int k, int s, int p);

void naive_conv_2d_float32(float32 *inp, float32 *oup,
		int x, int y, int oup_x, int oup_y, int sx, int sy,
	float32 *filter, int fw);

#ifdef __cplusplus
	}
#endif

#endif /* _CONV2D_H_ */