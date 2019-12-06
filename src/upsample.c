#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "data_types.h"
#include "debug_log.h"
#include "upsample.h"

static void (*_interpolation_handler)(void *inp,
	int x, int y, int n, void *oup, int dt);

feature_map_t *interpolation(feature_map_t *inp,
	int scale, int mode, const char *name)
{
	int i, i_ch_size, i_ch_mem_size, o_ch_size, o_ch_mem_size;
	feature_map_t *oup = NULL;
	if (!(scale > 0)) {
		QUICK_LOG_BAD_ARG(scale);
		return NULL;
	}
#ifdef ENABLE_MEMMGR
	oup = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!oup) {
		oup = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!oup) {
			QUICK_LOG_ERR_MEM_ALLOC(oup);
			return NULL;
		}
		oup->datatype = inp->datatype;
		oup->xsize    = inp->xsize * scale;
		oup->ysize    = inp->ysize * scale;
		oup->zsize    = inp->zsize;
		oup->data     = list_new_static(oup->zsize,
				oup->xsize * oup->ysize *
				sizeof_datatype(oup->datatype));
		if (!oup->data) {
			free(oup);
			QUICK_LOG_ERR_MEM_ALLOC(oup->data);
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	switch (mode) {
		case INTERPOLATION_CB:
			_interpolation_handler =
				naive_interpolation_checkerboard;
			break;
		case INTERPOLATION_NN:
			_interpolation_handler =
				naive_interpolation_nearestneighbor;
			break;
		default:
		/* Warning: unsupported method */
			return oup;
	}
	i_ch_size     = inp->xsize * inp->ysize;
	i_ch_mem_size = i_ch_size * sizeof_datatype(inp->datatype);
	o_ch_size     = oup->xsize * oup->ysize;
	o_ch_mem_size = o_ch_size * sizeof_datatype(oup->datatype);
	for (i = 0; i < inp->zsize; ++i)
	{
		_interpolation_handler(inp->data->mem + i_ch_mem_size * i,
						inp->xsize, inp->ysize, scale,
					oup->data->mem + o_ch_mem_size * i,
				inp->datatype);
	}
	return oup;
}

#define INTERPOLATION_CB_SUB_PROCESS(datatype) \
for (i = 0; i < y; ++i)                                   \
{                                                         \
	for (j = 0; j < x; ++j)                           \
	{                                                 \
		*((datatype*)oup + i * n * o_x + j * n) = \
			*((datatype*)inp + i * x + j);    \
	}                                                 \
}

void naive_interpolation_checkerboard(void *inp,
	int x, int y, int n, void *oup, int dt)
{
	int o_x = x * n;
	int o_y = y * n;
	int i, j;
	memset(oup, 0, o_x * o_y * sizeof_datatype(dt));
	switch (dt) {
		case DATATYPE_FLOAT32:
			INTERPOLATION_CB_SUB_PROCESS(float32);
			break;
		case DATATYPE_FLOAT64:
			INTERPOLATION_CB_SUB_PROCESS(float64);
			break;
		case DATATYPE_INT16:
			INTERPOLATION_CB_SUB_PROCESS(int16);
			break;
		case DATATYPE_INT32:
			INTERPOLATION_CB_SUB_PROCESS(int32);
			break;
		case DATATYPE_INT64:
			INTERPOLATION_CB_SUB_PROCESS(int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(datatype);
			break;
	}
}

#define INTERPOLATION_NN_SUB_PROCESS(datatype) \
for (i = 0; i < o_y; ++i)                                         \
{                                                                 \
	for (j = 0; j < o_x; ++j)                                 \
	{                                                         \
		*(((datatype*)oup) + i * o_x + j) =               \
			*(((datatype*)inp) +                      \
				(int)(i / n) * x + (int)(j / n)); \
	}                                                         \
}

void naive_interpolation_nearestneighbor(void *inp,
	int x, int y, int n, void *oup, int dt)
{
	int o_x = x * n;
	int o_y = y * n;
	int i, j;
	switch (dt) {
		case DATATYPE_FLOAT32:
			INTERPOLATION_NN_SUB_PROCESS(float32);
			break;
		case DATATYPE_FLOAT64:
			INTERPOLATION_NN_SUB_PROCESS(float64);
			break;
		case DATATYPE_INT16:
			INTERPOLATION_NN_SUB_PROCESS(int16);
			break;
		case DATATYPE_INT32:
			INTERPOLATION_NN_SUB_PROCESS(int32);
			break;
		case DATATYPE_INT64:
			INTERPOLATION_NN_SUB_PROCESS(int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(datatype);
			break;
	}
}
