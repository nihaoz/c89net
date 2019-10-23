#ifndef _CONV2D_H_
#define _CONV2D_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include <math.h>

#include "data_util.h"

typedef float conv_input_t;
typedef float conv_filter_t;

channel_float32 *conv2d_filter(float32 *data, int size);

channel_float32 *channel_conv2d(channel_float32 *inp, channel_float32 *filter, int s, int p);

/*
 * Calc output size after conv2d
 * inp: input size
 * k  : kernel size
 * s  : stride
 * p  : padding
 */
int _conv_2d_size_calc(int inp, int k, int s, int p);

/*
 * Pre-allocate memory for output by conv2d
 * k : kernel size
 * s : stride
 * p : padding
 */
channel_float32 *_conv_2d_malloc(channel_float32 *ch, int k, int s, int p);

void _conv_2d(conv_input_t *inp, conv_input_t *oup, int x, int y,
				int sx, int sy, int p, conv_filter_t *filter, int filter_width);

#ifdef __cplusplus
	}
#endif

#endif /* _CONV2D_H_ */