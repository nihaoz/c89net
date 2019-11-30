#ifndef _DATA_LAYER_H_
#define _DATA_LAYER_H_

/* Standrad 2-D conv */

#ifdef __cplusplus
	extern "C" {
#endif

#define PARA_TYPE_KERNEL 0
#define PARA_TYPE_BIAS   1
/* Batch Normalization */
#define PARA_TYPE_BN     2

#define PARA_BN_CHK 5

#include <stdio.h>

#include "data_util.h"
#include "list.h"

/*
 * basic data and parameter layer, using list_t to store data
 */
typedef struct {
	list_t *data;
	int datatype;
	int xsize;
	int ysize;
	int zsize;
} feature_map_t;

/*
 * (Spatial/Depthwise)Conv 2-D kernel:
 * xsize : kernel x-size
 * ysize : kernel y-size
 * zsize : kernel input channels
 * wsize : kernel output channels(avoid for Depthwise conv2d)
 * ----------------------------------------------------------
 *     Batch Normalization Parameters
 * | gamma | beta | mean | var | epsilon |
 *
 */

#define BN_OFFSET_GAMMA   0
#define BN_OFFSET_BETA    1
#define BN_OFFSET_MEAN    2
#define BN_OFFSET_VAR     3
#define BN_OFFSET_EPSILON 4

typedef struct {
	list_t *data;
	int type;
	int datatype;
	int xsize;
	int ysize;
	int zsize;
	int wsize;
} cnn_para_t;

feature_map_t *feature_map_by_channels(channel_t **chs,
				int n, const char *name);

/* feature_map_t *feature_map_serialized(int data_type, ...); */

feature_map_t *feature_map_clone(feature_map_t *l, const char *name);

channel_t *copy_channel_form_layer(feature_map_t *l, int id);

int feature_map_modify_shape(feature_map_t *l, int x, int y, int z);

feature_map_t *feature_map_flat(feature_map_t *l, const char *name);

/* Haven't implemented yet */
feature_map_t *feature_map_reshape(feature_map_t *l, int x, int y, int z);

feature_map_t *free_feature_map(feature_map_t *l);

cnn_para_t *free_cnn_parameters(cnn_para_t *l);

/* Testing functions */
cnn_para_t *load_cnn_conv2d_kernel(const char *filename,
		int ch_x, int ch_y, int ch_i, int ch_o, const char *name);

cnn_para_t *load_cnn_bias(const char *filename,
	int ch_i, const char *name);

cnn_para_t *load_cnn_batch_norm(const char *filename,
	int ch_i, const char *name);

float32 *bias_from_cnn_parameters(cnn_para_t *l);

channel_t *copy_kernel_form_layer(cnn_para_t *l, int id);

void debug_fprint_feature_map_info(feature_map_t *l, FILE *fp);

void debug_fprint_cnn_parameters_info(cnn_para_t *l, FILE *fp);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_LAYER_H_ */