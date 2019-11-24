#include <stdlib.h>
#include <string.h>

#include "data_types.h"
#include "debug_log.h"
#include "array_ops.h"

#define ELEM_OPS_ON_ARRAY(op, elem, arr, arrlen, datatype)        \
	for (i = 0; i < arrlen; ++i)                              \
	{                                                         \
		*((datatype*)arr + i) op ## = *(datatype*)elem;   \
	}

#define ARRAY_SUM(arr, arrlen, datatype, sum)               \
	for (i = 0; i < arrlen; ++i)                        \
	{                                                   \
		*(datatype*)sum += *((datatype*)arr + i);   \
	}

#define ARRAY_OPS(op, dst, src, arrlen, datatype)               \
	for (i = 0; i < arrlen; ++i)                            \
	{                                                       \
		*((datatype*)dst + i) += *((datatype*)src + i); \
	}

void soft_add_to_array(void *arr, int arrlen, void *x, int dt)
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

void soft_mul_to_array(void *arr, int arrlen, void *x, int dt)
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

void *add_to_array(void *arr, int arrlen, void *x, int dt)
{
	int i, mem_size = arrlen * sizeof_datatype(dt);
	void *buf = malloc(mem_size);
	if (!buf)
		return NULL;
	memcpy(buf, arr, mem_size);
	switch (dt) {
		case DATATYPE_FLOAT32:
			ELEM_OPS_ON_ARRAY(+, x, buf, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ELEM_OPS_ON_ARRAY(+, x, buf, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ELEM_OPS_ON_ARRAY(+, x, buf, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ELEM_OPS_ON_ARRAY(+, x, buf, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ELEM_OPS_ON_ARRAY(+, x, buf, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
	return buf;
}

void *mul_to_array(void *arr, int arrlen, void *x, int dt)
{
	int i, mem_size = arrlen * sizeof_datatype(dt);
	void *buf = malloc(mem_size);
	if (!buf)
		return NULL;
	memcpy(buf, arr, mem_size);
	switch (dt) {
		case DATATYPE_FLOAT32:
			ELEM_OPS_ON_ARRAY(*, x, buf, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ELEM_OPS_ON_ARRAY(*, x, buf, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ELEM_OPS_ON_ARRAY(*, x, buf, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ELEM_OPS_ON_ARRAY(*, x, buf, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ELEM_OPS_ON_ARRAY(*, x, buf, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
	return buf;
}

void array_sum(void *arr, int arrlen, void *x, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ARRAY_SUM(arr, arrlen, float32, x);
			break;
		case DATATYPE_FLOAT64:
			ARRAY_SUM(arr, arrlen, float64, x);
			break;
		case DATATYPE_INT16:
			ARRAY_SUM(arr, arrlen, int16, x);
			break;
		case DATATYPE_INT32:
			ARRAY_SUM(arr, arrlen, int32, x);
			break;
		case DATATYPE_INT64:
			ARRAY_SUM(arr, arrlen, int64, x);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}

void array_mean(void *arr, int arrlen, void *x, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ARRAY_SUM(arr, arrlen, float32, x);
			*(float32*)x /= arrlen;
			break;
		case DATATYPE_FLOAT64:
			ARRAY_SUM(arr, arrlen, float64, x);
			*(float64*)x /= arrlen;
			break;
		case DATATYPE_INT16:
			ARRAY_SUM(arr, arrlen, int16, x);
			*(int16*)x /= arrlen;
			break;
		case DATATYPE_INT32:
			ARRAY_SUM(arr, arrlen, int32, x);
			*(int32*)x /= arrlen;
			break;
		case DATATYPE_INT64:
			ARRAY_SUM(arr, arrlen, int64, x);
			*(int64*)x /= arrlen;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}

void array_ops_add(void *dst, void *src, int arrlen, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ARRAY_OPS(+, dst, src, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ARRAY_OPS(+, dst, src, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ARRAY_OPS(+, dst, src, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ARRAY_OPS(+, dst, src, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ARRAY_OPS(+, dst, src, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}

void array_ops_mul(void *dst, void *src, int arrlen, int dt)
{
	int i; /* for C89 style */
	switch (dt) {
		case DATATYPE_FLOAT32:
			ARRAY_OPS(*, dst, src, arrlen, float32);
			break;
		case DATATYPE_FLOAT64:
			ARRAY_OPS(*, dst, src, arrlen, float64);
			break;
		case DATATYPE_INT16:
			ARRAY_OPS(*, dst, src, arrlen, int16);
			break;
		case DATATYPE_INT32:
			ARRAY_OPS(*, dst, src, arrlen, int32);
			break;
		case DATATYPE_INT64:
			ARRAY_OPS(*, dst, src, arrlen, int64);
			break;
		default:
			QUICK_LOG_ERR_DATATYPE();
			break;
	}
}