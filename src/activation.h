#ifndef _ACTIVATION_H_
#define _ACTIVATION_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define ACTIVATION_NEW  0
#define ACTIVATION_SELF 1

#include "data_layer.h"

data_layer *activation_relu(data_layer *l, int f);

#ifdef __cplusplus
	}
#endif

#endif /* _ACTIVATION_H_ */