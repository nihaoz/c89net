#include <stdlib.h>

#include "conv2d.h"

int conv_2d_size_calc(int inp, int k, int s, int p)
{
	return (int)((inp - k + 2 * p) / s) + 1;
}

channel_t *conv_2d_malloc(channel_t *ch, int k, int s, int p)
{
	channel_t *new_ch = (channel_t*)malloc(sizeof(channel_t));
	if (!new_ch)
		return NULL;
	new_ch->datatype = DATATYPE_FLOAT32;
	new_ch->xsize = conv_2d_size_calc(ch->xsize, k, s, p);
	new_ch->ysize = conv_2d_size_calc(ch->ysize, k, s, p);
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
	channel_t *ch = conv_2d_malloc(inp, filter->xsize, s, p);
	if (!ch)
		return NULL;
	if (p) {
		/* Padding ops not implemented */
		fprintf(stderr, "%s: %d, channel_conv2d cannot support padding now\n",
					 __FILE__, __LINE__);
		exit(0);
	}
	naive_conv_2d(inp->data, ch->data, inp->xsize, inp->ysize,
					s, s, p, filter->data, filter->xsize);
	return ch;
}

void naive_conv_2d(float32 *inp, float32 *oup, int x, int y,
				int sx, int sy, int p, float32 *filter, int fw)
{
	int i, j, k, l;
	int half_fw = fw >> 1;
	float sum;
	int oup_x = x - (p << 1);
	int oup_y = y - (p << 1);
/* If enable OpenMP... Not performing well in processing small kernels
 * #ifdef ENABLE_OPENMP
 * 	omp_set_num_threads(num_conv_threads);
 * 	#pragma omp parallel for private(sum, j, k, l)
 * #endif
*/
	for (i = half_fw; i < y - half_fw; i += sy)
	{
		for (j = half_fw; j < x - half_fw; j += sx)
		{
			sum = 0; /* reset sum */
			for (k = -half_fw; k <= half_fw; ++k)
			{
				for (l = -half_fw; l <= half_fw; ++l)
				{
					sum += *(inp + (i + k) * x + (j + l)) *
						*(filter + (k + half_fw) * fw + (l + half_fw));
				}
			}
			int oup_i = ((i - half_fw) / sy);
			int oup_j = ((j - half_fw) / sx);
			*(oup + oup_i * oup_x + oup_j) = sum;
		}
	}
	return;
}
