#include <stdlib.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "data_util.h"
#include "fullyconn.h"
#include "spatial_conv2d.h"

extern void (*_fully_connected_float32)(void *inp,
		void *oup, void *w, void *b, int iw, int ow);

static void (*_fully_connected_handler)(void *inp,
		void *oup, void *w, void *b, int iw, int ow);

void naive_fully_connected_float32(float32 *inp, float32 *oup,
			float32 *w, float32 *b, int iw, int ow)
{
	int i, j;
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, j)
#endif
	for (i = 0; i < ow; ++i)
	{
		oup[i] = 0;
		for (j = 0; j < iw; ++j)
		{
			oup[i] += (*(inp + j)) * (*(w + iw * i + j));
		}
		oup[i] += b[i];
	}
	return;
}

feature_map_t *fully_connected(feature_map_t *inp,
		cnn_para_t *w, cnn_para_t *b, const char *name)
{
	/* Parameter check */
	if (inp->zsize != w->zsize) {
		QUICK_LOG_ERR_DATATYPE((inp->zsize != w->zsize));
		return NULL;
	}
	/* Datatype check */
	if (inp->datatype != w->datatype) {
		QUICK_LOG_ERR_DATATYPE((inp->datatype != w->datatype));
		return NULL;
	}
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_fully_connected_handler = _fully_connected_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
#ifdef ENABLE_MEMMGR
	feature_map_t *oup =
		(feature_map_t*)
			memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *oup = NULL;
#endif
	if (!oup) {
		oup = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!oup)
			return NULL;
		oup->datatype = inp->datatype;
		oup->xsize = 1;
		oup->ysize = 1;
		oup->zsize = w->wsize;
		oup->data = list_new_static(w->wsize,
			sizeof_datatype(oup->datatype));
		if (!oup->data) {
			free(oup);
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	_fully_connected_handler(inp->data->mem, oup->data->mem,
		w->data->mem, b->data->mem, w->zsize, w->wsize);
	return oup;
	/* return spatial_conv(inp, w, b, 1, 0, name); */
}

feature_map_t *spatial_conv2d_fully_connected(feature_map_t *inp,
			cnn_para_t *w, cnn_para_t *b, const char *name)
{
	return spatial_conv2d(inp, w, b, 1, 0, name);
}
