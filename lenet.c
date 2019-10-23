#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_util.h"
#include "image_bmp.h"
#include "conv2d.h"
#include "list.h"
#include "data_layer.h"
#include "std_conv2d.h"
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

#define DEFAULT_PARAMETERS_PATH "./lenet-parameters/"

#ifdef RUN_DEBUG
	#define DEBUG_OUTPUT_PATH "./debug/"
#endif

const char default_test_image[] = "test.bmp";

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
	channel_float32 *ch_gray = gray_to_channel(img_gray, GRAY_TO_CHANNEL_MNIST);
	img_gray = img_free_gray(img_gray);

	/* Load input layer */
	channel_float32 *chs[] = {ch_gray};
	data_layer *inp = data_layer_by_channels(chs, 1, "input");

	#ifdef RUN_DEBUG
		channel_float32 *debug_ch_from_inp = copy_channel_form_layer(inp, 0);
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_inp.txt");
		channel_dump_to_text(debug_ch_from_inp, global_string_buffer, 0);
	#endif

	/* Load LeNet L1 conv w */
	
	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "conv1_w");
	set_string_buffer_2(parameters_path, "conv1_w.bin")
	para_layer *conv1_w = load_conv2d_kernel_form_binary(
										global_string_buffer,
								5, 5, 1, 32, "conv1_w");

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "conv1_b");
	/* Load LeNet L1 conv b */
	set_string_buffer_2(parameters_path, "conv2_w.bin")
	para_layer *conv1_b = load_bias_form_binary(
								global_string_buffer,
								32, "conv1_b");
	/*
	 * L1 conv stride: 1 padding: 2
	 */
	data_layer *l1_conv = std_conv2d(inp, conv1_w, conv1_b, 1, 2);

	/*
	 * L1 relu
	 */
	data_layer *l1_conv_relu = activation_relu(l1_conv, 0);

	#ifdef RUN_DEBUG
		channel_float32 *debug_ch_from_conv1 = copy_channel_form_layer(l1_conv_relu, 0);
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_conv1.txt");
		channel_dump_to_text(debug_ch_from_conv1, global_string_buffer, 0);
	#endif
	/*
	 * L1 max pool 2 * 2
	 */
	data_layer *l1_conv_pool = max_pool2_2(l1_conv_relu);

	#ifdef RUN_DEBUG
		channel_float32 *debug_ch_from_pool1 = copy_channel_form_layer(l1_conv_pool, 0);
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_pool1.txt");
		channel_dump_to_text(debug_ch_from_pool1, global_string_buffer, 0);
	#endif

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "conv2_w");
	/* Load LeNet L2 conv w */
	set_string_buffer_2(parameters_path, "conv2_w.bin");
	para_layer *conv2_w = load_conv2d_kernel_form_binary(
										global_string_buffer,
								5, 5, 32, 64, "conv2_w");

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "conv2_b");
	/* Load LeNet L2 conv b */
	set_string_buffer_2(parameters_path, "conv2_b.bin");
	para_layer *conv2_b = load_bias_form_binary(
								global_string_buffer,
								64, "conv2_b");
	/*
	 * L2 conv stride: 1 padding: 2
	 */
	data_layer *l2_conv = std_conv2d(l1_conv_pool, conv2_w, conv2_b, 1, 2);

	/*
	 * L2 relu
	 */
	data_layer *l2_conv_relu = activation_relu(l2_conv, 0);

	/*
	 * L2 max pool 2 * 2
	 */
	data_layer *l2_conv_pool = max_pool2_2(l2_conv_relu);

	#ifdef RUN_DEBUG
		channel_float32 *debug_ch_from_pool2 = copy_channel_form_layer(l2_conv_pool, 0);
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_pool2.txt");
		channel_dump_to_text(debug_ch_from_pool2, global_string_buffer, 0);
	#endif

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "fc1_w");
	/* Load LeNet FC1 w */
	/*
	 * Loading parameters form big text file, may take a while...
	 */
	set_string_buffer_2(parameters_path, "fc1_w.bin");
	para_layer *fc1_w = load_conv2d_kernel_form_binary(
									global_string_buffer,
								1, 1, 3136, 1024, "fc1_w");

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "fc1_b");
	/* Load LeNet FC1 b */
	set_string_buffer_2(parameters_path, "fc1_b.bin");
	para_layer *fc1_b = load_bias_form_binary(
									global_string_buffer,
								1024, "fc1_b");
	/*
	 * Reshape l2_conv_pool layer for fully-connected layer
	 */
	/* int ops_state = data_layer_reshape(l2_conv_pool, 1, 1, 3136); */
	/* data_layer_reshape(l2_conv_pool, 1, 1, 3136); */
	data_layer *l2_flat = data_layer_flat(l2_conv_pool);

	#ifdef RUN_DEBUG
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_3136.txt");
		FILE *debug_fp_3136 = fopen(global_string_buffer, "w+");
		int debug_counter_3136;
		float32 *p_debug_3136 = (float32*)l2_flat->data->mem;
		for (debug_counter_3136 = 0; debug_counter_3136 < 3136; ++debug_counter_3136)
		{
			fprintf(debug_fp_3136, "%.8f ", *(p_debug_3136 + debug_counter_3136));
		}
		fclose(debug_fp_3136);
	#endif

	/*
	 * L fc1, by conv1_1 stride: 1 padding: 0
	 */
	data_layer *fc1 = std_conv2d(l2_flat, fc1_w, fc1_b, 1, 0);

	#ifdef RUN_DEBUG
		debug_fprint_data_layer_info(fc1, stdout);
	#endif

	/*
	 * L fc1 relu
	 */
	data_layer *fc1_relu = activation_relu(fc1, 1);

	#ifdef RUN_DEBUG
		set_string_buffer_2(DEBUG_OUTPUT_PATH, "debug_ch_from_fc1_relu.txt");
		FILE *debug_fp_fc1 = fopen(global_string_buffer, "w+");
		int debug_counter_fc1;
		float32 *p_debug_fc1 = (float32*)fc1_relu->data->mem;
		for (debug_counter_fc1 = 0; debug_counter_fc1 < 1024; ++debug_counter_fc1)
		{
			fprintf(debug_fp_fc1, "%.8f ", *(p_debug_fc1 + debug_counter_fc1));
		}
		fclose(debug_fp_fc1);
	#endif

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "fc2_w");
	/* Load LeNet FC2 w */
	set_string_buffer_2(parameters_path, "fc2_w.bin");
	para_layer *fc2_w = load_conv2d_kernel_form_binary(
									global_string_buffer,
								1, 1, 1024, 10, "fc2_w");

	format_log("Loading LeNet parameters: \33[1;32m%s\33[0m", "fc2_b");
	/* Load LeNet FC2 b */
	set_string_buffer_2(parameters_path, "fc2_b.bin");
	para_layer *fc2_b = load_bias_form_binary(
									global_string_buffer,
								10, "fc2_b");
	/*
	 * L fc2, by conv1_1 stride: 1 padding: 0
	 */
	data_layer *fc2 = std_conv2d(fc1_relu, fc2_w, fc2_b, 1, 0);

	/*
	 * Output result...
	 */

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
	return 0;
}