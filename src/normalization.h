#ifndef _NORMALIZATION_H_
#define _NORMALIZATION_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

void naive_batch_norm_float32(float32 *inp, int len, float32 *bnarg);

feature_map_t *batch_norm(feature_map_t *l, cnn_para_t *arg);

#ifdef __cplusplus
	}
#endif

#endif /* _NORMALIZATION_H_ */