/* Fully connections */
#ifndef _FULLYCOON_H_
#define _FULLYCOON_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

void naive_fully_connected(float32 *inp, float32 *oup,
					float32 *w, float32 *b, int iw, int ow);

feature_map_t *fully_connected(feature_map_t *inp, cnn_para_t *w, cnn_para_t *b);

/* Test */
feature_map_t *spatial_conv_fully_connected(feature_map_t *inp,
									cnn_para_t *w, cnn_para_t *b);
#ifdef __cplusplus
	}
#endif

#endif /* _FULLYCOON_H_ */