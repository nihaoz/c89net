#include <stdio.h>

#include "activation.h"
#include "data_util.h"
#include "list.h"

feature_map_t *activation_relu(feature_map_t *l, int f)
{
	feature_map_t *n_l;
	if (f == ACTIVATION_SELF) {
		n_l = l;
	} else {
		n_l = feature_map_clone(l, NULL);
		if (!n_l)
			return NULL;
	}
	int i, layer_size = l->xsize * l->ysize * l->zsize;
	float32 *p = (float32*)n_l->data->mem;
	for (i = 0; i < layer_size; ++i)
	{
		p[i] = p[i] > 0 ? p[i] : 0;
	}
	return n_l;
}
