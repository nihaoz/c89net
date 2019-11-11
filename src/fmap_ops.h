#ifndef _FMAP_OPS_H_
#define _FMAP_OPS_H_

/* Simple operations on feature maps */

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

feature_map_t *feature_map_bias(feature_map_t *inp, cnn_para_t *bias);

#ifdef __cplusplus
	}
#endif

#endif /* _FMAP_OPS_H_ */