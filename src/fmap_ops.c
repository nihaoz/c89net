#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "array_ops.h"
#include "fmap_ops.h"

#define BIAS_OPS_BY_DATATYPE(datatype)                                  \
for (i = 0; i < bias->zsize; ++i)                                       \
{                                                                       \
	for (j = 0; j < ch_size; ++j)                                   \
	{                                                               \
		*(datatype*)(inp->data->mem +                           \
		(ch_size * i + j) * sizeof(datatype)) +=                \
		*(datatype*)(bias->data->mem + i * sizeof(datatype));   \
	}                                                               \
}

feature_map_t *feature_map_bias(feature_map_t *inp, cnn_para_t *bias)
{
	int i, j, ch_size;
	/* Parameter check */
	if (inp->zsize != bias->zsize) {
		QUICK_LOG_ERR_DATATYPE((inp->zsize != bias->zsize));
		return NULL;
	}
	/* Datatype check */
	if (inp->datatype != bias->datatype) {
		QUICK_LOG_ERR_DATATYPE((inp->datatype != bias->datatype));
		return NULL;
	}
	ch_size = inp->xsize * inp->ysize;
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			BIAS_OPS_BY_DATATYPE(float32);
			break;
		case DATATYPE_FLOAT64:
			BIAS_OPS_BY_DATATYPE(float64);
			break;
		case DATATYPE_INT16:
			BIAS_OPS_BY_DATATYPE(int16);
			break;
		case DATATYPE_INT32:
			BIAS_OPS_BY_DATATYPE(int32);
			break;
		case DATATYPE_INT64:
			BIAS_OPS_BY_DATATYPE(int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
	return inp;
}