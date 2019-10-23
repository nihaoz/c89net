#ifndef _DATA_UTIL_H_
#define _DATA_UTIL_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * Preprocessing default datatype is float32
 */

/* #define ELEM_FORMAT_OCT */
#define ELEM_FORMAT_DEF 0
#define ELEM_FORMAT_DEC 0
#define ELEM_FORMAT_HEX 1

typedef float  float32;
typedef double float64;

#include <stdio.h>

#include "image_util.h"

typedef struct
{
	float32 *data;
	int xsize;
	int ysize;
} channel_float32;

/* Single channel, [0, 1] */
#define GRAY_TO_CHANNEL_MNIST  0
/* Multi channel, [-1, 1] */
#define GRAY_TO_CHANNEL_COMMON 1

#define CHANNEL_NORM_NEW  0
#define CHANNEL_NORM_SELF 1

channel_float32 *new_channel_float32(int x, int y);

channel_float32 *channel_norm(channel_float32 *ch, float32 l, float32 r, int f);

channel_float32 *clone_channel_float32(channel_float32 *ch);

/* Convert a gray image into a 2-D float32 channel, pxls val form [0, 255] to [-1, 1]*/
channel_float32 *gray_to_channel(gray_obj *gray, int mod);

/* Convert a normalized 2-D float32 channel to gray image */
gray_obj *normalized_channel_to_gray(channel_float32 *ch);

channel_float32 *free_channel_float32(channel_float32 *ch);

int channel_dump_to_text(channel_float32 *ch, const char* filename, int format);

void debug_fprint_ch_info(FILE *fp, channel_float32 *ch);

/* dst = dst + src */
int channel_add(channel_float32 *dst, channel_float32 *src);

int channel_bias(channel_float32 *ch, float32 bias);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_UTIL_H_ */