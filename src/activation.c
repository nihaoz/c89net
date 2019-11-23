#include <stdio.h>

#include "debug_log.h"
#include "activation.h"
#include "data_util.h"
#include "list.h"

extern void (*_activation_relu_float32)(void *inp, int len);
extern void (*_activation_relu6_float32)(void *inp, int len);
extern void (*_activation_leaky_relu_float32)(void *inp,
					void *alpha, int len);

static void (*_activation_relu_handler)(void *inp, int len);
static void (*_activation_relu6_handler)(void *inp, int len);
static void (*_activation_leaky_relu_handler)(void *inp,
					void *alpha, int len);

feature_map_t *activation_relu(feature_map_t *inp)
{
	int len = inp->xsize * inp->ysize * inp->zsize;
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_activation_relu_handler = _activation_relu_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
	_activation_relu_handler(inp->data->mem, len);
	return inp;
}

feature_map_t *activation_relu6(feature_map_t *inp)
{
	int len = inp->xsize * inp->ysize * inp->zsize;
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_activation_relu6_handler = _activation_relu6_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
	_activation_relu6_handler(inp->data->mem, len);
	return inp;
}

feature_map_t *activation_leaky_relu(feature_map_t *inp, void *alpha)
{
	int len = inp->xsize * inp->ysize * inp->zsize;
	switch (inp->datatype) {
		case DATATYPE_FLOAT32:
			_activation_leaky_relu_handler =
				_activation_leaky_relu_float32;
			break;
		default:
			QUICK_LOG_ERR_DATATYPE(inp->datatype);
	}
	_activation_leaky_relu_handler(inp->data->mem, alpha, len);
	return inp;
}

void navie_activation_relu_float32(float32 *inp, int len)
{
	int i; /* Happy C89 */
	for (i = 0; i < len; ++i)
	{
		inp[i] = inp[i] > 0 ? inp[i] : 0;
	}
}

void navie_activation_relu6_float32(float32 *inp, int len)
{
	int i; /* Happy C89 */
	for (i = 0; i < len; ++i)
	{
		inp[i] = inp[i] > 0 ? inp[i] : 0;
		inp[i] = inp[i] > 6 ? 6 : inp[i];
	}
}

void navie_activation_leaky_relu_float32(float32 *inp,
				float32 *alpha, int len)
{
	int i; /* Happy C89 */
	for (i = 0; i < len; ++i)
	{
		inp[i] = inp[i] < 0 ? inp[i] * (*alpha) : inp[0];
	}
}