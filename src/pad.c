#include <stdlib.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "pad.h"

feature_map_t *pad_surround(feature_map_t *l, int p, const char *name)
{
	if (!l || p < 0)
		return NULL;
#ifdef ENABLE_MEMMGR
	feature_map_t *pad = (feature_map_t*)memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *pad = NULL;
#endif
	if (!pad) {
		pad = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pad)
			return NULL;
		pad->datatype = l->datatype;
		pad->xsize = l->xsize + p + p;
		pad->ysize = l->ysize + p + p;
		pad->zsize = l->zsize;
		pad->data  = list_new_static(l->zsize, sizeof(float32) * pad->xsize * pad->ysize);
		if (!pad->data) {
			free(pad);
			return NULL;
		}
		list_set_name(pad->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, pad);
#endif
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
	return pad;
}
