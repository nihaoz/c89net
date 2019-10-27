#include <stdlib.h>

#include "conv2d.h"

int _conv_2d_size_calc(int inp, int k, int s, int p)
{
	return (int)((inp - k + 2 * p) / s) + 1;
}

channel_t *_conv_2d_malloc(channel_t *ch, int k, int s, int p)
{
	channel_t *new_ch = (channel_t*)malloc(sizeof(channel_t));
	if (!new_ch)
		return NULL;
	new_ch->datatype = DATATYPE_FLOAT32;
	new_ch->xsize = _conv_2d_size_calc(ch->xsize, k, s, p);
	new_ch->ysize = _conv_2d_size_calc(ch->ysize, k, s, p);
	new_ch->data  = (float32*)malloc(sizeof(float32) * new_ch->xsize * new_ch->ysize);
	if (!new_ch->data) {
		free(new_ch);
		return NULL;
	}
	return new_ch;
}

channel_t *conv2d_filter(float32 *data, int size)
{
	int k = (int)sqrt(size);
	if (k * k - size)
		return NULL;
	channel_t *ch = new_channel(DATATYPE_FLOAT32, k, k);
	if (!ch)
		return NULL;
	int i;
	for (i = 0; i < size; ++i)
	{
		ch->data[i] = data[i];
	}
	return ch;
}

channel_t *channel_conv2d(channel_t *inp, channel_t *filter, int s, int p)
{
	if (datatype_check(inp->datatype, DATATYPE_FLOAT32) ||
			datatype_check(filter->datatype, DATATYPE_FLOAT32)) {
		fprintf(stderr, "2-D channel conv only support float32 data for now\n");
		return NULL;
	}
	channel_t *ch = _conv_2d_malloc(inp, filter->xsize, s, p);
	if (!ch)
		return NULL;
	if (p) {
		/* Padding ops not implemented */
		fprintf(stderr, "%s: %d, channel_conv2d cannot support padding now\n",
					 __FILE__, __LINE__);
		exit(0);
	}
	_conv_2d(inp->data, ch->data, inp->xsize, inp->ysize,
					s, s, p, filter->data, filter->xsize);
	return ch;
}

void _conv_2d(conv_input_t *inp, conv_input_t *oup, int x, int y,
				int sx, int sy, int p, conv_filter_t *filter, int filter_width)
{
	int i, j, k, l;
	int half_filter_width = filter_width >> 1;
	float sum;
	int oup_x = x - (p << 1);
	int oup_y = y - (p << 1);
/* If enable OpenMP... Not performing well in processing small kernels
 * #ifdef ENABLE_OPENMP
 * 	omp_set_num_threads(num_conv_threads);
 * 	#pragma omp parallel for private(sum, j, k, l)
 * #endif
*/
	for (i = half_filter_width; i < y - half_filter_width; i += sy)
	{
		for (j = half_filter_width; j < x - half_filter_width; j += sx)
		{
			sum = 0; /* reset sum */
			for (k = -half_filter_width; k <= half_filter_width; ++k)
			{
				for (l = -half_filter_width; l <= half_filter_width; ++l)
				{
					sum += *(inp + (i + k) * x + (j + l)) *
						*(filter + (k + half_filter_width) * filter_width + (l + half_filter_width));
				}
			}
			int oup_i = ((i - half_filter_width) / sy);
			int oup_j = ((j - half_filter_width) / sx);
			*(oup + oup_i * oup_x + oup_j) = sum;
		}
	}
	return;
}
