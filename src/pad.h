#ifndef _PAD_H_
#define _PAD_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

/*
 * Default padding with 0
 */

feature_map_t *pad_surround(feature_map_t *l, int p, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _PAD_H_ */