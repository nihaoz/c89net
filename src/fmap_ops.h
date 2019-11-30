#ifndef _FMAP_OPS_H_
#define _FMAP_OPS_H_

/* Simple operations on feature maps */

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

int feature_map_shape_cmp(feature_map_t *a, feature_map_t *b);

feature_map_t *feature_map_bias(feature_map_t *inp, cnn_para_t *bias);

feature_map_t *feature_map_add(feature_map_t *dst, feature_map_t *src);

#ifdef __cplusplus
	}
#endif

#endif /* _FMAP_OPS_H_ */