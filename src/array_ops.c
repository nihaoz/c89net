#include "data_types.h"
#include "debug_log.h"
#include "array_ops.h"

#define ELEM_OPS_ON_ARRAY(op, elem, arr, arrlen, datatype)        \
	for (i = 0; i < arrlen; i++)                              \
	{                                                         \
		*((datatype*)arr + i) op ## = *(datatype*)elem;   \
	}

void add_to_array(void *arr, int arrlen, void *x, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ELEM_OPS_ON_ARRAY(+, x, arr, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ELEM_OPS_ON_ARRAY(+, x, arr, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ELEM_OPS_ON_ARRAY(+, x, arr, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ELEM_OPS_ON_ARRAY(+, x, arr, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ELEM_OPS_ON_ARRAY(+, x, arr, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}

void mul_to_array(void *arr, int arrlen, void *x, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ELEM_OPS_ON_ARRAY(*, x, arr, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ELEM_OPS_ON_ARRAY(*, x, arr, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ELEM_OPS_ON_ARRAY(*, x, arr, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ELEM_OPS_ON_ARRAY(*, x, arr, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ELEM_OPS_ON_ARRAY(*, x, arr, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}
