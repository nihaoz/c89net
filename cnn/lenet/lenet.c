#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "image_bmp.h"
#include "data_layer.h"
#include "spatial_conv2d.h"
#include "fullyconn.h"
#include "pad.h"
#include "pool.h"
#include "activation.h"
#include "debug_log.h"

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

#define DEFAULT_PARAMETERS_PATH "./cnn/lenet/lenet-parameters/"
const char default_test_image[] = "./cnn/lenet/test.bmp";

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

	/* Load LeNet L1 conv w */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "conv1_w");
	set_string_buffer_2(parameters_path, "conv1_w.bin")
	cnn_para_t *conv1_w = load_cnn_conv2d_kernel(
					global_string_buffer,
					5, 5, 1, 32, "conv1_w");

	/* Load LeNet L1 conv b */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "conv1_b");
	set_string_buffer_2(parameters_path, "conv2_w.bin")
	cnn_para_t *conv1_b = load_cnn_bias(
					global_string_buffer,
					32, "conv1_b");

	/* Load LeNet L2 conv w */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "conv2_w");
	set_string_buffer_2(parameters_path, "conv2_w.bin");
	cnn_para_t *conv2_w = load_cnn_conv2d_kernel(
					global_string_buffer,
					5, 5, 32, 64, "conv2_w");

	/* Load LeNet L2 conv b */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "conv2_b");
	set_string_buffer_2(parameters_path, "conv2_b.bin");
	cnn_para_t *conv2_b = load_cnn_bias(
					global_string_buffer,
					64, "conv2_b");

	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "fc1_w");
	/* Load LeNet FC1 w */
	/*
	 * Loading parameters form big text file, may take a while...
	 */
	set_string_buffer_2(parameters_path, "fc1_w.bin");
	cnn_para_t *fc1_w = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 3136, 1024, "fc1_w");

	/* Load LeNet FC1 b */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "fc1_b");
	set_string_buffer_2(parameters_path, "fc1_b.bin");
	cnn_para_t *fc1_b = load_cnn_bias(
					global_string_buffer,
					1024, "fc1_b");

	/* Load LeNet FC2 w */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "fc2_w");
	set_string_buffer_2(parameters_path, "fc2_w.bin");
	cnn_para_t *fc2_w = load_cnn_conv2d_kernel(
					global_string_buffer,
					1, 1, 1024, 10, "fc2_w");

	/* Load LeNet FC2 b */
	format_log(LOG_INFO, "Loading LeNet parameters: \33[1;32m%s\33[0m", "fc2_b");
	set_string_buffer_2(parameters_path, "fc2_b.bin");
	cnn_para_t *fc2_b = load_cnn_bias(
					global_string_buffer,
					10, "fc2_b");

	format_log(LOG_INFO, "Loading LeNet parameters finished");

	feature_map_t *l1_conv, *l1_conv_relu, *l1_conv_pool, *l2_conv, *l2_conv_relu, \
		*l2_conv_pool, *l2_flat, *fc1, *fc1_relu, *fc2;

	/*
	 * L1 conv stride: 1 padding: 2
	 */
	l1_conv = spatial_conv2d(inp, conv1_w, conv1_b, 1, 2, 0, "conv1");
	/*
	 * L1 relu
	 */
	l1_conv_relu = activation_relu(l1_conv);
	/*
	 * L1 max pool 2 * 2
	 */
	l1_conv_pool = max_pool2_2(l1_conv_relu, "pool1");
	/*
	 * L2 conv stride: 1 padding: 2
	 */
	l2_conv = spatial_conv2d(l1_conv_pool, conv2_w, conv2_b, 1, 2, 0, "conv2");
	/*
	 * L2 relu
	 */
	l2_conv_relu = activation_relu(l2_conv);
	/*
	 * L2 max pool 2 * 2
	 */
	l2_conv_pool = max_pool2_2(l2_conv_relu, "pool2");
	/*
	 * Reshape l2_conv_pool layer for fully-connected layer
	 */
	l2_flat = feature_map_flat(l2_conv_pool, "flat");
	/*
	 * L fc1, by conv1_1 stride: 1 padding: 0
	 */

	fc1 = fully_connected(l2_flat, fc1_w, fc1_b, "fc1");
	/*
	 * L fc1 relu
	 */
	fc1_relu = activation_relu(fc1);
	/*
	 * L fc2, by conv1_1 stride: 1 padding: 0
	 */
	fc2 = fully_connected(fc1_relu, fc2_w, fc2_b, "fc2");

	/*
	 * Output result...
	 */
	format_log(LOG_INFO, "Computing finished!");
	float32 *p = (float32*)fc2->data->mem;
	float32 max_value = 0;
	int     max_index = 0;
	printf("\nPrint all output of LeNet(no softmax):\n");
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

#ifdef ENABLE_MEMMGR
	debug_fprint_memmgr_list(stdout);
	memmgr_clear();
#endif
	return 0;
}