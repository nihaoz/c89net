#include <stdlib.h>

#include "data_util.h"
#include "fullyconn.h"
#include "spatial_conv.h"

extern void (*_fully_connected_float32)(float32 *inp,
		float32 *oup, float32 *w, float32 *b, int iw, int ow);

void naive_fully_connected(float32 *inp, float32 *oup,
					float32 *w, float32 *b, int iw, int ow)
{
	int i, j;
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, j)
#endif
	for (i = 0; i < ow; ++i)
	{
		for (j = 0; j < iw; ++j)
		{
			oup[i] += (*(inp + j)) * (*(w + iw * i + j));
		}
		oup[i] += b[i];
	}
	return;
}

feature_map_t *fully_connected(feature_map_t *inp,
							cnn_para_t *w, cnn_para_t *b)
{
	feature_map_t *oup = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!oup)
		return NULL;
	oup->datatype = inp->datatype;
	oup->xsize = 1;
	oup->ysize = 1;
	oup->zsize = w->wsize;
	oup->data = list_new_static(w->wsize, sizeof(float32));
	if (!oup->data) {
		free(oup);
		return NULL;
	}
	_fully_connected_float32((float32*)inp->data->mem,
				(float32*)oup->data->mem, (float32*)w->data->mem,
			(float32*)b->data->mem, w->zsize, w->wsize);
	return oup;
	/* return spatial_conv(inp, w, b, 1, 0); */
}

feature_map_t *spatial_conv_fully_connected(feature_map_t *inp,
							cnn_para_t *w, cnn_para_t *b)
{
	return spatial_conv(inp, w, b, 1, 0);
}
