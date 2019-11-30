#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "array_ops.h"
#include "fmap_ops.h"

int feature_map_shape_cmp(feature_map_t *a, feature_map_t *b)
{
	if (a->xsize != b->xsize ||
		a->ysize != b->ysize ||
		a->zsize != b->zsize)
		return -1;
	return 0;
}

feature_map_t *feature_map_bias(feature_map_t *inp, cnn_para_t *bias)
{
	int i, ch_size, ch_mem_size, bias_dt_size;
	/* Parameter check */
	if (inp->zsize != bias->zsize) {
		QUICK_LOG_BAD_ARG((inp->zsize != bias->zsize));
		return NULL;
	}
	/* Datatype check */
	if (inp->datatype != bias->datatype) {
		QUICK_LOG_ERR_DATATYPE((inp->datatype != bias->datatype));
		return NULL;
	}
	ch_size      = inp->xsize * inp->ysize;
	ch_mem_size  = ch_size * sizeof_datatype(inp->datatype);
	bias_dt_size = sizeof_datatype(bias->datatype);
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < bias->zsize; ++i)
	{
		add_to_array(inp->data->mem + ch_mem_size * i,
			ch_size, (bias->data->mem + bias_dt_size * i),
			inp->datatype);
	}
	return inp;
}

feature_map_t *feature_map_add(feature_map_t *dst, feature_map_t *src)
{
	int i, ch_size, ch_mem_size;
	/* Parameter check */
	if (feature_map_shape_cmp(dst, src)) {
		QUICK_LOG_BAD_ARG(feature_map_shape_cmp(dst, src));
		return NULL;
	}
	/* Datatype check */
	if (dst->datatype != src->datatype) {
		QUICK_LOG_ERR_DATATYPE((dst->datatype != src->datatype));
		return NULL;
	}
	ch_size     = dst->xsize * dst->ysize;
	ch_mem_size = ch_size * sizeof_datatype(dst->datatype);
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < dst->zsize; ++i)
	{
		array_ops_add(dst->data->mem + ch_mem_size * i,
			src->data->mem + ch_mem_size * i, ch_size,
			dst->datatype);
	}
	return dst;
}

feature_map_t *feature_map_mul(feature_map_t *dst, feature_map_t *src)
{
	int i, ch_size, ch_mem_size;
	/* Parameter check */
	if (feature_map_shape_cmp(dst, src)) {
		QUICK_LOG_BAD_ARG(feature_map_shape_cmp(dst, src));
		return NULL;
	}
	/* Datatype check */
	if (dst->datatype != src->datatype) {
		QUICK_LOG_ERR_DATATYPE((dst->datatype != src->datatype));
		return NULL;
	}
	ch_size     = dst->xsize * dst->ysize;
	ch_mem_size = ch_size * sizeof_datatype(dst->datatype);
#ifdef ENABLE_OPENMP
	#pragma omp parallel for private(i)
#endif
	for (i = 0; i < dst->zsize; ++i)
	{
		array_ops_mul(dst->data->mem + ch_mem_size * i,
			src->data->mem + ch_mem_size * i, ch_size,
			dst->datatype);
	}
	return dst;
}