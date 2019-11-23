/*
 * Global function configuration
 */

#ifndef _GLOBAL_FN_CFG_H_
#define _GLOBAL_FN_CFG_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * Set implemention for _batch_norm_xxxx
 * Default:(go to the end of this segment)
 * naive_pppp_ffff_xxxx, Ref: normalization.h
 */
#include "normalization.h"

void (*_batch_norm_float32)(float32 *inp, int len, float32 *bnarg) = \
	naive_batch_norm_float32;

/*
 * Set implemention for _activation_ffff_xxxx
 * Default:(go to the end of this segment)
 * naive_pppp_ffff_xxxx, Ref: normalization.h
 */
#include "activation.h"

void (*_activation_relu_float32)(float32 *inp, int len) = \
	navie_activation_relu_float32;

void (*_activation_relu6_float32)(float32 *inp, int len) = \
	navie_activation_relu6_float32;

void (*_activation_leaky_relu_float32)(float32 *inp,
	float32 *alpha, int len) = navie_activation_leaky_relu_float32;

/*
 * Set implemention for _conv_2d_float32
 * Default:(go to the end of this segment)
 * naive_pppp_ffff_xxxx, Ref: conv2d.h
 */
#include "conv2d.h"

void (*_conv_2d_float32)(float32 *inp, float32 *oup,
		int x, int y, int oup_x, int oup_y, int sx, int sy,
	float32 *filter, int fw) = naive_conv_2d_float32;

/*
 * Set implemention for _fully_connected_xxxx
 * Default:(go to the end of this segment)
 * naive_pppp_ffff_xxxx, Ref: fullyconn.h
 */

#ifdef SET_GCC_BUILTIN_VEX
	#include "vxsf_fullyconn.h"
	void (*_fully_connected_float32)(float32 *inp, float32 *oup,
				float32 *w, float32 *b, int iw, int ow) = \
		v4sf_fully_connected_float32;
/* #elif ... */
#else /* Set default value */
	#include "fullyconn.h"
	void (*_fully_connected_float32)(float32 *inp, float32 *oup,
				float32 *w, float32 *b, int iw, int ow) = \
		naive_fully_connected_float32;
#endif

#ifdef __cplusplus
	}
#endif

#endif /* _GLOBAL_FN_CFG_H_ */