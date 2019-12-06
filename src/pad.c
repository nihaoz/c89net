#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "data_types.h"
#include "pad.h"

#define GENERIC_PADDING_MEMCPY \
	memcpy((pad->data->mem) + p_ch_mem_size * c +                       \
	(i + p - poffset) * p_row_mem_size + (p + j - poffset) * dtsize,    \
	l->data->mem + l_ch_mem_size * c + i * l_row_mem_size + j * dtsize, \
	dtsize);

feature_map_t *pad_surround(feature_map_t *l, int p,
			int offset, const char *name)
{
	int i, j, c, dtsize = sizeof_datatype(l->datatype);
	int l_ch_size, l_ch_mem_size, l_row_mem_size,
		p_ch_size, p_ch_mem_size, p_row_mem_size;
	int poffset = offset > 0 ? 1 : 0;
	int soffset = offset ? 1 : 0;
	feature_map_t *pad = NULL;
	if (!l || p < 0) {
		QUICK_LOG_BAD_ARG(pad);
		return NULL;
	}
	l_ch_size      = l->xsize * l->ysize;
	l_ch_mem_size  = l_ch_size * dtsize;
	l_row_mem_size = l->xsize * dtsize;
#ifdef ENABLE_MEMMGR
	pad = (feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#endif
	if (!pad) {
		pad = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!pad) {
			QUICK_LOG_ERR_MEM_ALLOC(pad);
			return NULL;
		}
		pad->datatype = l->datatype;
		pad->xsize = l->xsize + p + p - soffset;
		pad->ysize = l->ysize + p + p - soffset;
		pad->zsize = l->zsize;
		pad->data  = list_new_static(l->zsize,
				pad->xsize * pad->ysize *
				sizeof_datatype(l->datatype));
		if (!pad->data) {
			free(pad);
			QUICK_LOG_ERR_MEM_ALLOC(pad->data);
			return NULL;
		}
		list_set_name(pad->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, pad);
#endif
	}
	p_ch_size      = pad->xsize * pad->ysize;
	p_ch_mem_size  = p_ch_size * dtsize;
	p_row_mem_size = pad->xsize * dtsize;
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
