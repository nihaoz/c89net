/* Fully connections */
#ifndef _FULLYCONN_H_
#define _FULLYCONN_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

void naive_fully_connected_float32(float32 *inp, float32 *oup,
					float32 *w, float32 *b, int iw, int ow);

feature_map_t *fully_connected(feature_map_t *inp,
				cnn_para_t *w, cnn_para_t *b, const char *name);

/* Test */
feature_map_t *spatial_conv2d_fully_connected(feature_map_t *inp,
					cnn_para_t *w, cnn_para_t *b, const char *name);
#ifdef __cplusplus
	}
#endif

#endif /* _FULLYCONN_H_ */