#ifdef ENABLE_OPENMP
	#include <omp.h>
#endif
#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "pointwise_conv2d.h"

/*
 * Ref: global_function_config.h
 * void (*_conv_2d)(void *inp, void *oup, int x, int y,
 *			int sx, int sy, int p, void *filter, int filter_width);
 */

/* #include "global_function_config.h" */

extern void (*_conv_2d_float32)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

/* _conv_2d_handler for current processing datatype */
static void (*_conv_2d_handler)(void *inp, void *oup, int x, int y,
			int sx, int sy, int p, void *filter, int filter_width);

feature_map_t *pointwise_conv2d(feature_map_t *inp, cnn_para_t *kernel,
					cnn_para_t *bias, const char *name)
{
	/* Parameter check */
	if (inp->zsize != kernel->zsize) {
		QUICK_LOG_ERR_DATATYPE((inp->zsize != kernel->zsize));
		return NULL;
	}
	/* Datatype check */
	if (inp->datatype != kernel->datatype) {
		QUICK_LOG_ERR_DATATYPE((inp->datatype != kernel->datatype));
		return NULL;
	}
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_conv_2d_handler = _conv_2d_float32;
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
		oup->xsize = inp->xsize;
		oup->ysize = inp->ysize;
		oup->zsize = kernel->wsize;
		oup->data  = list_new_static(kernel->wsize,
				sizeof_datatype(oup->datatype) *
						oup->xsize * oup->ysize);
		if (!oup->data) {
			free(oup);
			return NULL;
		}
		list_set_name(oup->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, oup);
#endif
	}
	/*
	 * Not implemented yet...
	 */
	return NULL;
}