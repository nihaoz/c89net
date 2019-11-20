#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "data_types.h"
#include "pad.h"

#define GENERIC_PADDING_MEMCPY \
	memcpy((pad->data->mem) + pad_ch_mem_size * c +                     \
	(i + p - offset) * p_row_mem_size + (p + j - offset) * dtsize,      \
	l->data->mem + l_ch_mem_size * c + i * l_row_mem_size + j * dtsize, \
	dtsize);

feature_map_t *pad_surround(feature_map_t *l, int p,
			int offset, const char *name)
{
	if (!l || p < 0)
		return NULL;
#ifdef ENABLE_MEMMGR
	feature_map_t *pad =
		(feature_map_t*)
			memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *pad = NULL;
#endif
	if (!pad) {
		pad = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pad)
			return NULL;
		pad->datatype = l->datatype;
		pad->xsize = l->xsize + p + p - offset;
		pad->ysize = l->ysize + p + p - offset;
		pad->zsize = l->zsize;
		pad->data  = list_new_static(l->zsize,
				pad->xsize * pad->ysize *
				sizeof_datatype(l->datatype));
		if (!pad->data) {
			free(pad);
			return NULL;
		}
		list_set_name(pad->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, pad);
#endif
	}
	int i, j, c, dtsize = sizeof_datatype(l->datatype);
	int l_ch_size = l->xsize * l->ysize;
	int l_ch_mem_size = l_ch_size * dtsize;
	int l_row_mem_size = l->xsize * dtsize;
	int pad_ch_size = pad->xsize * pad->ysize;
	int pad_ch_mem_size = pad_ch_size * dtsize;
	int p_row_mem_size = pad->xsize * dtsize;
	for (c = 0; c < l->zsize; ++c) {
		for (i = 0; i < l->ysize; ++i)
		{
			for (j = 0; j < l->xsize; ++j)
			{
				GENERIC_PADDING_MEMCPY;
			}
		}
	}
	return pad;
}
