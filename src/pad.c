#include <stdlib.h>

#include "pad.h"

/*
 * Checkout this!
 * What to do when padding set to 0 ?
 */
data_layer *pad_surround(data_layer *l, int p, const char *name)
{
	if (!l || p < 0)
		return NULL;
	data_layer *pad = (data_layer*)malloc(sizeof(data_layer));
	if (!pad)
		return NULL;
	pad->xsize = l->xsize + p + p;
	pad->ysize = l->ysize + p + p;
	pad->zsize = l->zsize;
	pad->data  = list_new_static(l->zsize, sizeof(float32) * pad->xsize * pad->ysize);
	if (!pad->data) {
		free(pad);
		return NULL;
	}
	int i, j, c;
	int l_ch_size = l->xsize * l->ysize;
	int pad_ch_size = pad->xsize * pad->ysize;
	for (c = 0; c < l->zsize; ++c) {
		for (i = 0; i < l->ysize; ++i)
		{
			for (j = 0; j < l->xsize; ++j)
			{
				*((float32*)(pad->data->mem) + pad_ch_size * c + (i + p) * pad->xsize + p + j) =
					*((float32*)l->data->mem + l_ch_size * c + i * l->xsize + j);
			}
		}
	}
	list_set_name(pad->data, name);
	return pad;
}
