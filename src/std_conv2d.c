#ifdef __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include "pad.h"
#include "std_conv2d.h"

data_layer *std_conv2d(data_layer *inp, para_layer *kernel,
							para_layer *bias, int s, int p)
{
	/* Parameter check */
	if (inp->zsize != kernel->zsize) {
		fprintf(stderr, "Conv parameter error %s: %d, input channel %d != \
			kernel input channel %d \n", __FILE__, __LINE__, inp->zsize, kernel->zsize);
		return NULL;
	}
	data_layer *inp_pad = pad_surround(inp, p, "tmp");
	if (!inp_pad)
		return NULL;
	data_layer *oup = (data_layer*)malloc(sizeof(data_layer));
	if (!oup) {
		free_data_layer(inp_pad);
		return NULL;
	}
	oup->xsize = _conv_2d_size_calc(inp->xsize, kernel->xsize, s, p);
	oup->ysize = _conv_2d_size_calc(inp->ysize, kernel->ysize, s, p);
	oup->zsize = kernel->wsize;
	oup->data  = list_new_static(kernel->wsize, sizeof(float32) * oup->xsize * oup->ysize);
	if (!oup->data) {
		free(oup);
		free_data_layer(inp_pad);
		return NULL;
	}
	int oup_ch_size = oup->xsize * oup->ysize;
	float32 *out_buf = (float32*)malloc(sizeof(float32) * oup_ch_size);
	if (!out_buf) {
		free_data_layer(oup);
		free_data_layer(inp_pad);
		return NULL;
	}
	int p_ch_mem_eval = inp_pad->xsize * inp_pad->ysize * sizeof(float32);
	int o_ch_mem_eval = oup->xsize * oup->ysize * sizeof(float32);
	int k_mem_eval    = kernel->xsize * kernel->ysize * kernel->zsize * sizeof(float32);
	int k_ch_mem_eval = kernel->xsize * kernel->ysize * sizeof(float32);
	int i, j, k;
	double supersum = 0;
	for (i = 0; i < kernel->wsize; ++i)
	{
		for (j = 0; j < kernel->zsize; ++j)
		{
			_conv_2d((conv_input_t*)(inp_pad->data->mem + j * p_ch_mem_eval),
							out_buf, inp_pad->xsize, inp_pad->ysize, s, s, p, 
					(conv_filter_t*)(kernel->data->mem + (i * k_mem_eval) + j * k_ch_mem_eval),
				kernel->xsize);
			for (k = 0; k < oup_ch_size; ++k)
			{
				*(((float32*)(oup->data->mem + i * o_ch_mem_eval)) + k) += out_buf[k];
			}
		}
	}
	free(out_buf);
	if (!bias){
		free_data_layer(inp_pad);
		return oup;
	} else {
		float32 *p_bias = bias_data_from_layer(bias);
		for (i = 0; i < kernel->wsize; ++i)
		{
			for (k = 0; k < oup_ch_size; ++k)
			{
				*(((float32*)(oup->data->mem + i * o_ch_mem_eval)) + k) += p_bias[i];
			}
		}
	}
	free_data_layer(inp_pad);
	return oup;
}

#ifdef __cplusplus
	}
#endif