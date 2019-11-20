#include <stdio.h>

#include "activation.h"
#include "data_util.h"
#include "list.h"

feature_map_t *activation_relu(feature_map_t *l)
{
	int i, layer_size = l->xsize * l->ysize * l->zsize;
	float32 *p = (float32*)l->data->mem;
	for (i = 0; i < layer_size; ++i)
	{
		p[i] = p[i] > 0 ? p[i] : 0;
	}
	return l;
}

feature_map_t *activation_relu6(feature_map_t *l)
{
	int i, layer_size = l->xsize * l->ysize * l->zsize;
	float32 *p = (float32*)l->data->mem;
	for (i = 0; i < layer_size; ++i)
	{
		p[i] = p[i] > 0 ? p[i] : 0;
		p[i] = p[i] > 6 ? 6 : p[i];
	}
	return l;
}
