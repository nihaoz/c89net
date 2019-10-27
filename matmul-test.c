#include <stdio.h>
#include <stdlib.h>

#include "data_layer.h"
#include "spatial_conv.h"

int main(int argc, char const *argv[])
{
	data_layer *x = (data_layer*)malloc(sizeof(data_layer));
	x->xsize = 1;
	x->ysize = 1;
	x->zsize = 3;
	x->data  = list_new_static(x->zsize,
		sizeof(float32) * x->xsize * x->ysize);

	float32 *p_x = (float32*)x->data->mem;

	p_x[0] = 1;
	p_x[1] = 2;
	p_x[2] = 3;

	int i, j;
	for (i = 0; i < x->xsize * x->ysize * x->zsize; ++i)
	{
		printf("x[%d]: %f\n", i, *(p_x + i));
	}

	para_layer *w = (para_layer*)malloc(sizeof(para_layer));
	w->xsize = 1;
	w->ysize = 1;
	w->zsize = 3;
	w->wsize = 2;
	w->data  = list_new_static(w->xsize * w->ysize * w->zsize * w->wsize, sizeof(float32));

	float32 *p_w = (float32*)w->data->mem;

	p_w[0] = 1;
	p_w[1] = 3;
	p_w[2] = 5;
	p_w[3] = 2;
	p_w[4] = 4;
	p_w[5] = 6;

	for (i = 0; i < w->xsize * w->ysize * w->zsize * w->wsize; ++i)
	{
		printf("w[%d]: %f\n", i, *(p_w + i));
	}

	data_layer *y = spatial_conv(x, w, NULL, 1, 0);

	float32 *p_y = (float32*)y->data->mem;

	debug_fprint_data_layer_info(y, stdout);

	for (i = 0; i < y->xsize * y->ysize * y->zsize; ++i)
	{
		printf("y[%d]: %f\n", i, *(p_y + i));
	}

	return 0;
}