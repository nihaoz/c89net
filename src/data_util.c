#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_util.h"
#include "image_util.h"

static double _round_c89(double number)
{
    return (number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5);
}

const char _elem_print_format[2][6] = {
	{"%.8f "},
	{"%08x "}
};

channel_float32 *free_channel_float32(channel_float32 *ch)
{
	if (ch) {
		if (ch->data)
			free(ch->data);
		free(ch);
	}
	return NULL;
}

channel_float32 *gray_to_channel(gray_obj *gray, int mod)
{
	channel_float32 *ch = (channel_float32*)malloc(sizeof(channel_float32));
	if (!ch)
		return NULL;
	ch->xsize = gray->xsize;
	ch->ysize = gray->ysize;
	ch->data  = (float32*)malloc(sizeof(float32) * ch->xsize * ch->ysize);
	if (!ch->data) {
		free(ch);
		return NULL;
	}
	int i, j;
	for (i = 0; i < gray->ysize; ++i)
	{
		for (j = 0; j < gray->xsize; ++j)
		{
			switch (mod) {
				case GRAY_TO_CHANNEL_MNIST:
					ch->data[i * ch->xsize + j] = (float32)gray->data[i * gray->xsize + j];
					ch->data[i * ch->xsize + j] /= 255;
					break;
				case GRAY_TO_CHANNEL_COMMON:
				default:
					ch->data[i * ch->xsize + j] = (float32)gray->data[i * gray->xsize + j];
					ch->data[i * ch->xsize + j] /= 255;
					ch->data[i * ch->xsize + j] -= 0.5;
					ch->data[i * ch->xsize + j] *= 2.0;
					break;
			}
		}
	}
	return ch;
}

channel_float32 *new_channel_float32(int x, int y)
{
	channel_float32 *ch = (channel_float32*)malloc(sizeof(channel_float32));
	if (!ch)
		return NULL;
	ch->xsize = x;
	ch->ysize = y;
	ch->data  = (float32*)malloc(sizeof(float32) * ch->xsize * ch->ysize);
	if (!ch->data) {
		free(ch);
		return NULL;
	}
	return ch;
}

channel_float32 *clone_channel_float32(channel_float32 *ch)
{
	channel_float32 *clone = (channel_float32*)malloc(sizeof(channel_float32));
	if (!clone)
		return NULL;
	clone->xsize = ch->xsize;
	clone->ysize = ch->ysize;
	clone->data  = (float32*)malloc(sizeof(float32) * clone->xsize * clone->ysize);
	if (!clone->data) {
		free(clone);
		return NULL;
	}
	int i, j;
	for (i = 0; i < clone->ysize; ++i)
	{
		for (j = 0; j < clone->xsize; ++j)
		{
			clone->data[i * clone->xsize + j] = ch->data[i * ch->xsize + j];
		}
	}
	return clone;
}


channel_float32 *channel_norm(channel_float32 *ch, float32 l, float32 r, int f)
{
	float32 gap = r - l;
	if (gap <= 0) 
		return NULL;
	channel_float32 *n_ch;
	if (f == CHANNEL_NORM_SELF)
		n_ch = ch;
	else
		n_ch = new_channel_float32(ch->xsize, ch->ysize);
	float32 min = 0, max = 0;
	int i, n = ch->xsize * ch->ysize;
	for (i = 0; i < n; ++i)
	{
		min = ch->data[i] < min ? ch->data[i] : min;
		max = ch->data[i] > max ? ch->data[i] : max;
	}
	float32 fac = (max -min) / gap;
	float32 off = l - (min / fac);
	if (!fac) {
		for (i = 0; i < n; ++i)
		{
			n_ch->data[i] = 0;
		}
		return n_ch;
	}
	for (i = 0; i < n; ++i)
	{
		n_ch->data[i] /= fac;
		n_ch->data[i] += off;
	}
	return n_ch;
}

gray_obj *normalized_channel_to_gray(channel_float32 *ch)
{
	gray_obj *gray = (gray_obj*)malloc(sizeof(gray_obj));
	if (!gray)
		return NULL;
	gray->xsize = ch->xsize;
	gray->ysize = ch->ysize;
	gray->data = (byte*)malloc(sizeof(byte) * gray->xsize * gray->ysize);
	int i, j;
	float32 t;
	for (i = 0; i < ch->ysize; ++i)
	{
		for (j = 0; j < ch->xsize; ++j)
		{
			t = ch->data[i * ch->xsize + j];
			t += 1.0;
			t /= 2.0;
			t *= 255;
			#ifdef CONFIG_STD_C89
				gray->data[i * gray->xsize + j] = (byte)_round_c89(t);
			#else
				gray->data[i * gray->xsize + j] = (byte)round(t);
			#endif
		}
	}
	return gray;
}

int channel_dump_to_text(channel_float32 *ch, const char* filename, int format)
{
	FILE *fp = fopen(filename, "w+");
	if (!fp)
		return -1;
	int i, j;
	for (i = 0; i < ch->ysize; ++i)
	{
		for (j = 0; j < ch->xsize; ++j)
		{
			fprintf(fp, _elem_print_format[format], *(ch->data + i * ch->xsize + j));
		}
	}
	fclose(fp);
	return 0;
}

void debug_fprint_ch_info(FILE *fp, channel_float32 *ch)
{
	fprintf(fp, "2-D float32 channel info: \n");
	fprintf(fp, "Size(X * Y): %d * %d\n", ch->xsize, ch->ysize);
	return;
}

int channel_add(channel_float32 *dst, channel_float32 *src)
{
	if (dst->xsize != src->xsize || dst->ysize != src->ysize)
		return -1;
	int i, j;
	for (i = 0; i < dst->ysize; ++i)
	{
		for (j = 0; j < dst->xsize; ++j)
		{
			dst->data[i * dst->xsize + j] += src->data[i * src->xsize + j];
		}
	}
	return 0;
}

int channel_bias(channel_float32 *ch, float32 bias)
{
	int i, j;
	for (i = 0; i < ch->ysize; ++i)
	{
		for (j = 0; j < ch->xsize; ++j)
		{
			ch->data[i * ch->xsize + j] += bias;
		}
	}
	return 0;
}
