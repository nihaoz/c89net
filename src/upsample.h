#ifndef _UPSAMPLE_H_
#define _UPSAMPLE_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "data_layer.h"

/* Interpolation mode */
/* Checkerboard */
#define INTERPOLATION_CB 1
/* Nearest Neighbor */
#define INTERPOLATION_NN 2

feature_map_t *interpolation(feature_map_t *inp,
	int scale, int mode, const char *name);

void naive_interpolation_checkerboard(void *inp,
	int x, int y, int n, void *oup, int dt);

void naive_interpolation_nearestneighbor(void *inp,
	int x, int y, int n, void *oup, int dt);

#ifdef __cplusplus
	}
#endif

#endif /* _UPSAMPLE_H_ */