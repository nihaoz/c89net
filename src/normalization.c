#include <math.h>

#include "debug_log.h"
#include "normalization.h"

extern void (*_batch_norm_float32)(void *inp, int len, void *bnarg);

static void (*_batch_norm_handler)(void *inp, int len, void *bnarg);

void naive_batch_norm_float32(float32 *inp, int len, float32 *bnarg)
{
	float32 gamma   = *(bnarg + BN_OFFSET_GAMMA),
		beta    = *(bnarg + BN_OFFSET_BETA),
		mean    = *(bnarg + BN_OFFSET_MEAN),
		var     = *(bnarg + BN_OFFSET_VAR),
		epsilon = *(bnarg + BN_OFFSET_EPSILON);
	float32 frac = sqrt(var + epsilon);
	int i;
	for (i = 0; i < len; ++i)
	{
		*(inp + i) = (gamma * 
				(*(inp + i) - mean) / frac)
			+ beta;
	}
	return;
}

feature_map_t *batch_norm(feature_map_t *l, cnn_para_t *arg)
{
	int ch_size, ch_mem_size, i;
	/* Parameter check */
	if (l->zsize != arg->zsize) {
		QUICK_LOG_ERR_DATATYPE((l->zsize != arg->zsize));
		return NULL;
	}
	/* Datatype check */
	if (l->datatype != arg->datatype) {
		QUICK_LOG_ERR_DATATYPE((l->datatype != arg->datatype));
		return NULL;
	}
	switch (l->datatype) {
		case DATATYPE_FLOAT32:
			_batch_norm_handler = _batch_norm_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(l->datatype);
	}
	ch_size     = l->xsize * l->ysize;
	ch_mem_size = ch_size * sizeof_datatype(l->datatype);
	for (i = 0; i < arg->zsize; ++i)
	{
		_batch_norm_handler(l->data->mem + ch_mem_size * i,
				ch_size, arg->data->mem + PARA_BN_CHK * 
			sizeof_datatype(arg->datatype) * i);
	}
	return l;
}