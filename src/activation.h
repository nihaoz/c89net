#ifndef _ACTIVATION_H_
#define _ACTIVATION_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

feature_map_t *activation_relu(feature_map_t *l);

feature_map_t *activation_relu6(feature_map_t *l);

#ifdef __cplusplus
	}
#endif

#endif /* _ACTIVATION_H_ */