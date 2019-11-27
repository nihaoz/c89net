#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "image_bmp.h"
#include "data_layer.h"
#include "spatial_conv2d.h"
#include "depthwise_conv2d.h"
#include "pointwise_conv2d.h"
#include "fullyconn.h"
#include "normalization.h"
#include "pad.h"
#include "pool.h"
#include "activation.h"
#include "debug_log.h"
#include "array_ops.h"

#define RUN_DEBUG
#undef  RUN_DEBUG

#define INPUT_IMG_X 28
#define INPUT_IMG_Y 28

/* Using this to convert path(filename) */
char global_string_buffer[256] = {0};
#define set_string_buffer_2(a, b) \
	sprintf(global_string_buffer, "%s%s", a, b);
#define set_string_buffer_3(a, b, c) \
	sprintf(global_string_buffer, "%s%s%s", a, b, c);

char parameters_path[256] = {0};

#define DEFAULT_PARAMETERS_PATH "./cnn/tinynet/parameters/"
const char default_test_image[] = "./cnn/tinynet/test.bmp";

#ifdef RUN_DEBUG
	#define DEBUG_OUTPUT_PATH "./debug/"
#endif

int main(int argc, char const *argv[])
{
	/*
	 * Initialization...
	 */
	strcpy(parameters_path, DEFAULT_PARAMETERS_PATH);
	const char *input_name;

	if (argc !=2 ) {
		input_name = default_test_image;
	} else {
		input_name = argv[1];
	}

	int i, j; /* Happy C89 */
	/* rgb_obj  *img   = img_new_rgb(INPUT_IMG_X, INPUT_IMG_Y, -1); */
	rgb_obj *img = img_read_bmp(input_name);
	gray_obj *img_gray = img_new_gray(INPUT_IMG_X, INPUT_IMG_Y, -1);
	img_rgb_split(img->data, INPUT_IMG_X * INPUT_IMG_Y,
					img_gray->data, img_gray->data, img_gray->data);
	img = img_free_rgb(img);
	channel_t *ch_gray = gray_to_channel_float32(img_gray, GRAY_TO_CHANNEL_MNIST);
	img_gray = img_free_gray(img_gray);

#ifdef ENABLE_MEMMGR 
	memmgr_init();
#endif

	/* Load input layer */
	channel_t *chs[] = {ch_gray};
	feature_map_t *inp = feature_map_by_channels(chs, 1, "input");

	#ifdef RUN_DEBUG
		channel_float32 *debug_ch_from_inp = copy_channel_form_layer(inp, 0);
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_inp.txt");
		channel_dump_to_text(debug_ch_from_inp, global_string_buffer, 0);
	#endif

	/* Load TinyNet L1 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv1_dw");
	set_string_buffer_2(parameters_path, "conv1_dw.bin")
	cnn_para_t *conv1_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 1, 1, "conv1_dw");

	/* Load TinyNet L1 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv1_pw");
	set_string_buffer_2(parameters_path, "conv1_pw.bin")
	cnn_para_t *conv1_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 1, 4, "conv1_pw");
	/* Load TinyNet L1 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv1_bn");
	set_string_buffer_2(parameters_path, "conv1_bn.bin")
	cnn_para_t *conv1_bn = load_cnn_batch_norm(
					global_string_buffer,
					4, "conv1_bn");

	/* Load TinyNet L2 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv2_dw");
	set_string_buffer_2(parameters_path, "conv2_dw.bin")
	cnn_para_t *conv2_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 4, 1, "conv2_dw");

	/* Load TinyNet L2 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv2_pw");
	set_string_buffer_2(parameters_path, "conv2_pw.bin")
	cnn_para_t *conv2_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 4, 4, "conv2_pw");
	/* Load TinyNet L2 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv2_bn");
	set_string_buffer_2(parameters_path, "conv2_bn.bin")
	cnn_para_t *conv2_bn = load_cnn_batch_norm(
					global_string_buffer,
					4, "conv2_bn");

	/* Load TinyNet L3 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv3_dw");
	set_string_buffer_2(parameters_path, "conv3_dw.bin")
	cnn_para_t *conv3_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 4, 1, "conv3_dw");

	/* Load TinyNet L3 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv3_pw");
	set_string_buffer_2(parameters_path, "conv3_pw.bin")
	cnn_para_t *conv3_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 4, 8, "conv3_pw");
	/* Load TinyNet L3 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv3_bn");
	set_string_buffer_2(parameters_path, "conv3_bn.bin")
	cnn_para_t *conv3_bn = load_cnn_batch_norm(
					global_string_buffer,
					8, "conv3_bn");

	/* Load TinyNet L4 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv4_dw");
	set_string_buffer_2(parameters_path, "conv4_dw.bin")
	cnn_para_t *conv4_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 8, 1, "conv4_dw");

	/* Load TinyNet L4 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv4_pw");
	set_string_buffer_2(parameters_path, "conv4_pw.bin")
	cnn_para_t *conv4_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 8, 8, "conv4_pw");
	/* Load TinyNet L4 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv4_bn");
	set_string_buffer_2(parameters_path, "conv4_bn.bin")
	cnn_para_t *conv4_bn = load_cnn_batch_norm(
					global_string_buffer,
					8, "conv4_bn");

	/* Load TinyNet L5 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv5_dw");
	set_string_buffer_2(parameters_path, "conv5_dw.bin")
	cnn_para_t *conv5_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 8, 1, "conv5_dw");

	/* Load TinyNet L5 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv5_pw");
	set_string_buffer_2(parameters_path, "conv5_pw.bin")
	cnn_para_t *conv5_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 8, 16, "conv5_pw");
	/* Load TinyNet L5 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv5_bn");
	set_string_buffer_2(parameters_path, "conv5_bn.bin")
	cnn_para_t *conv5_bn = load_cnn_batch_norm(
					global_string_buffer,
					16, "conv5_bn");

	/* Load TinyNet L6 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv6_dw");
	set_string_buffer_2(parameters_path, "conv6_dw.bin")
	cnn_para_t *conv6_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 16, 1, "conv6_dw");

	/* Load TinyNet L6 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv6_pw");
	set_string_buffer_2(parameters_path, "conv6_pw.bin")
	cnn_para_t *conv6_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 16, 32, "conv6_pw");
	/* Load TinyNet L6 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv6_bn");
	set_string_buffer_2(parameters_path, "conv6_bn.bin")
	cnn_para_t *conv6_bn = load_cnn_batch_norm(
					global_string_buffer,
					32, "conv6_bn");

	/* Load TinyNet L7 conv dw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv7_dw");
	set_string_buffer_2(parameters_path, "conv7_dw.bin")
	cnn_para_t *conv7_dw = load_cnn_conv2d_kernel(
					global_string_buffer,
					3, 3, 32, 1, "conv7_dw");

	/* Load TinyNet L7 conv pw */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv7_pw");
	set_string_buffer_2(parameters_path, "conv7_pw.bin")
	cnn_para_t *conv7_pw = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 32, 64, "conv7_pw");
	/* Load TinyNet L7 batch norm */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "conv7_bn");
	set_string_buffer_2(parameters_path, "conv7_bn.bin")
	cnn_para_t *conv7_bn = load_cnn_batch_norm(
					global_string_buffer,
					64, "conv7_bn");
	/* Load TinyNet FC w */
	set_string_buffer_2(parameters_path, "fc_w.bin");
	cnn_para_t *fc_w = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 64, 10, "fc_w");

	/* Load TinyNet FC b */
	format_log(LOG_INFO, "Loading TinyNet parameters: \33[1;32m%s\33[0m", "fc_b");
	set_string_buffer_2(parameters_path, "fc_b.bin");
	cnn_para_t *fc_b = load_cnn_bias(
					global_string_buffer,
					10, "fc_b");

	feature_map_t *l1_conv_dw, *l1_conv_pw, *l1_conv_bn,
		*l2_conv_dw, *l2_conv_pw, *l2_conv_bn,
		*l3_conv_dw, *l3_conv_pw, *l3_conv_bn,
		*l4_conv_dw, *l4_conv_pw, *l4_conv_bn,
		*l5_conv_dw, *l5_conv_pw, *l5_conv_bn,
		*l6_conv_dw, *l6_conv_pw, *l6_conv_bn,
		*l7_conv_dw, *l7_conv_pw, *l7_conv_bn,
		*l7_pool, *fc;

	l1_conv_dw = depthwise_conv2d(inp, conv1_dw, 1, 1, 0, "conv1_dw");
	/* l1_conv_pw = spatial_conv2d(l1_conv_dw, conv1_pw, NULL, 1, 0, 0, "conv1_pw"); */
	l1_conv_pw = pointwise_conv2d(l1_conv_dw, conv1_pw, NULL, "conv1_pw");
	l1_conv_bn = batch_norm(l1_conv_pw, conv1_bn);
	l1_conv_bn = activation_relu6(l1_conv_bn);

	l2_conv_dw = depthwise_conv2d(l1_conv_bn, conv2_dw, 2, 1, 1, "conv2_dw");
	l2_conv_pw = pointwise_conv2d(l2_conv_dw, conv2_pw, NULL, "conv2_pw");
	l2_conv_bn = batch_norm(l2_conv_pw, conv2_bn);
	l2_conv_bn = activation_relu6(l2_conv_bn);

	l3_conv_dw = depthwise_conv2d(l2_conv_bn, conv3_dw, 1, 1, 0, "conv3_dw");
	l3_conv_pw = pointwise_conv2d(l3_conv_dw, conv3_pw, NULL, "conv3_pw");
	l3_conv_bn = batch_norm(l3_conv_pw, conv3_bn);
	l3_conv_bn = activation_relu6(l3_conv_bn);

	l4_conv_dw = depthwise_conv2d(l3_conv_bn, conv4_dw, 1, 1, 0, "conv4_dw");
	l4_conv_pw = pointwise_conv2d(l4_conv_dw, conv4_pw, NULL, "conv4_pw");
	l4_conv_bn = batch_norm(l4_conv_pw, conv4_bn);
	l4_conv_bn = activation_relu6(l4_conv_bn);

	l5_conv_dw = depthwise_conv2d(l4_conv_bn, conv5_dw, 2, 1, 1, "conv5_dw");
	l5_conv_pw = pointwise_conv2d(l5_conv_dw, conv5_pw, NULL, "conv5_pw");
	l5_conv_bn = batch_norm(l5_conv_pw, conv5_bn);
	l5_conv_bn = activation_relu6(l5_conv_bn);

	l6_conv_dw = depthwise_conv2d(l5_conv_bn, conv6_dw, 1, 1, 0, "conv6_dw");
	l6_conv_pw = pointwise_conv2d(l6_conv_dw, conv6_pw, NULL, "conv6_pw");
	l6_conv_bn = batch_norm(l6_conv_pw, conv6_bn);
	l6_conv_bn = activation_relu6(l6_conv_bn);

	l7_conv_dw = depthwise_conv2d(l6_conv_bn, conv7_dw, 2, 1, 0, "conv7_dw");
	l7_conv_pw = pointwise_conv2d(l7_conv_dw, conv7_pw, NULL, "conv7_pw");
	l7_conv_bn = batch_norm(l7_conv_pw, conv7_bn);
	l7_conv_bn = activation_relu6(l7_conv_bn);

	l7_pool = channel_avg(l7_conv_bn, "l7_pool");

	fc = fully_connected(l7_pool, fc_w, fc_b, "fc");

	/*
	 * Output result...
	 */
	format_log(LOG_INFO, "Computing finished!");
	float32 *p = (float32*)fc->data->mem;
	float32 max_value = 0;
	int     max_index = 0;
	printf("\nPrint all output of TinyNet(no softmax):\n");
	for (i = 0; i < 10; ++i)
	{
		if (*(p + i) > max_value) {
			max_value = *(p + i);
			max_index = i;
		}
		printf("Index %d: %f\n", i, *(p + i));
	}
	printf("MAX INDEX: \33[1;31m%d\33[0m, it's the prediction result of [%s]\n",
				max_index, input_name);

#define FREE_RESOURCES(l) \
	free_cnn_parameters(conv##l##_dw);\
	free_cnn_parameters(conv##l##_pw);\
	free_cnn_parameters(conv##l##_bn);\

	FREE_RESOURCES(1);
	FREE_RESOURCES(2);
	FREE_RESOURCES(3);
	FREE_RESOURCES(4);
	FREE_RESOURCES(5);
	FREE_RESOURCES(6);
	FREE_RESOURCES(7);

	free_cnn_parameters(fc_w);
	free_cnn_parameters(fc_b);
	free_feature_map(inp);
	free_channel(ch_gray);

#ifdef ENABLE_MEMMGR
	/* debug_fprint_memmgr_list(stdout); */
	memmgr_clear();
#endif
	return 0;
}