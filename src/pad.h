#ifndef _PAD_H_
#define _PAD_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

#define PADDING_NAME_BUF_LEN  256
#define CONV_PAD_NAME_SURFFIX "_pad"
/*
 * Default padding with 0
 */

feature_map_t *pad_surround(feature_map_t *l, int p,
			int offset, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _PAD_H_ */