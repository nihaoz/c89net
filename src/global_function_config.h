/*
 * Global function configuration
 */

#ifndef _GLOBAL_FN_CFG_H_
#define _GLOBAL_FN_CFG_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * naive_conv_2d, Ref: conv2d.h
 */
#include "conv2d.h"

void (*_conv_2d_float32)(float32 *inp, float32 *oup, int x, int y,
				int sx, int sy, int p, float32 *filter, int fw) = \
		naive_conv_2d;

/*
 * naive_fully_connected, Ref: fullyconn.h
 */
#include "fullyconn.h"
void (*_fully_connected_float32)(float32 *inp,
				float32 *oup, float32 *w, float32 *b, int iw, int ow) = \
		naive_fully_connected;

#endif /* _GLOBAL_FN_CFG_H_ */