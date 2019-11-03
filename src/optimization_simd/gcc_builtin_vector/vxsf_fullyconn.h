#ifndef _VXSF_FULLYCONN_H_
#define _VXSF_FULLYCONN_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef SET_GCC_BUILTIN_VEX

#ifndef float32
	#define float32 float
#endif

void v4sf_fully_connected(float32 *inp, float32 *oup,
					float32 *w, float32 *b, int iw, int ow);

#endif /* SET_GCC_BUILTIN_VEX */

#ifdef __cplusplus
	}
#endif

#endif /* _VXSF_FULLYCONN_H_ */