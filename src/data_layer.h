#ifndef _DATA_LAYER_H_
#define _DATA_LAYER_H_

/* Standrad 2-D conv */

#ifdef __cplusplus
	extern "C" {
#endif

#define PARA_TYPE_KERNEL 0
#define PARA_TYPE_BIAS   1

#include <stdio.h>

#include "data_util.h"
#include "list.h"

/*
 * basic data and parameter layer, using list_t to store data
 */
typedef struct {
	list_t *data;
	int xsize;
	int ysize;
	int zsize;
} data_layer;

/*
 * Conv 2-D kernel:
 * xsize : kernel x-size
 * ysize : kernel y-size
 * zsize : kernel input channels
 * wsize : kernel output channels
 */
typedef struct {
	list_t *data;
	int type;
	int xsize;
	int ysize;
	int zsize;
	int wsize;
} para_layer;

data_layer *data_layer_by_channels(channel_float32 **chs, int n, const char *name);

data_layer *data_layer_clone(data_layer *l, const char *name);

channel_float32 *copy_channel_form_layer(data_layer *l, int id);

int data_layer_modify_shape(data_layer *l, int x, int y, int z);

data_layer *data_layer_flat(data_layer *l);

/* Haven't implemented yet */
data_layer *data_layer_reshape(data_layer *l, int x, int y, int z);

data_layer *free_data_layer(data_layer *l);
para_layer *free_para_layer(para_layer *l);

/* Testing functions */

para_layer *load_conv2d_kernel_form_text(const char *filename,
				int ch_x, int ch_y, int ch_i, int ch_o, const char *name);

para_layer *load_conv2d_kernel_form_binary(const char *filename,
				int ch_x, int ch_y, int ch_i, int ch_o, const char *name);

para_layer *load_bias_form_text(const char *filename, int ch_x, const char *name);

para_layer *load_bias_form_binary(const char *filename, int ch_x, const char *name);

float32 *bias_data_from_layer(para_layer *l);

channel_float32 *copy_kernel_form_layer(para_layer *l, int id);

void debug_fprint_data_layer_info(data_layer *l, FILE *fp);
void debug_fprint_para_layer_info(para_layer *l, FILE *fp);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_LAYER_H_ */