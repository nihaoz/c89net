#ifndef _ACTIVATION_H_
#define _ACTIVATION_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

feature_map_t *activation_softmax(feature_map_t *inp);

feature_map_t *activation_relu(feature_map_t *inp);

feature_map_t *activation_relu6(feature_map_t *inp);

feature_map_t *activation_leaky_relu(feature_map_t *inp, void *alpha);

void navie_activation_softmax_float32(float32 *inp, int len);
void navie_activation_relu_float32(float32 *inp, int len);
void navie_activation_relu6_float32(float32 *inp, int len);
void navie_activation_leaky_relu_float32(float32 *inp,
				float32 *alpha, int len);

#ifdef __cplusplus
	}
#endif

#endif /* _ACTIVATION_H_ */