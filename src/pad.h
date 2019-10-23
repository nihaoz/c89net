#ifndef _PAD_H_
#define _PAD_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

/*
 * Default padding with 0
 */

data_layer *pad_surround(data_layer *l, int p, const char *name);

#ifdef __cplusplus
	}
#endif

#endif /* _PAD_H_ */