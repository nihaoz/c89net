#include <stdio.h>

#include "image_bmp.h"
#include "data_layer.h"
#include "memmgr.h"

#define INPUT_IMG_X 28
#define INPUT_IMG_Y 28

const char input_name[] = "./cnn/lenet/test.bmp";

int main(int argc, char const *argv[])
{
	rgb_obj *img = img_read_bmp(input_name);
	gray_obj *img_gray = img_new_gray(INPUT_IMG_X, INPUT_IMG_Y, -1);
	img_rgb_split(img->data, INPUT_IMG_X * INPUT_IMG_Y,
					img_gray->data, img_gray->data, img_gray->data);
	img = img_free_rgb(img);
	channel_t *ch_gray = gray_to_channel_float32(img_gray, GRAY_TO_CHANNEL_MNIST);
	img_gray = img_free_gray(img_gray);
	channel_t *chs[] = {ch_gray};
	feature_map_t *inp = feature_map_by_channels(chs, 1, "input");
	cnn_para_t *conv1_b = load_cnn_bias("./cnn/lenet/lenet-parameters/conv1_b.bin",
								32, "conv1_b");

	debug_fprint_feature_map_info(inp, stdout);
	debug_fprint_cnn_parameters_info(conv1_b, stdout);

	int stat = memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, inp);

	memmgr_init();
	stat = memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, inp);
	stat = memmgr_add_record(MEMMGR_REC_TYPE_CNN_PARA, conv1_b);

	/* memmgr_clear(); */

	debug_fprint_memmgr_info(stdout);

	debug_fprint_memmgr_list(stdout);

	return 0;
}