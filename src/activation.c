#include <stdio.h>

#include "activation.h"
#include "data_util.h"
#include "list.h"

data_layer *activation_relu(data_layer *l, int f)
{
	data_layer *n_l;
	if (f == ACTIVATION_SELF) {
		n_l = l;
	} else {
		n_l = data_layer_clone(l, NULL);
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
