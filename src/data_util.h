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

#include <stdio.h>

#include "data_types.h"
#include "image_util.h"

typedef struct
{
	float32 *data;
	int datatype;
	int xsize;
	int ysize;
} channel_t;

/* Single channel, [0, 1] */
#define GRAY_TO_CHANNEL_MNIST  0
/* Multi channel, [-1, 1] */
#define GRAY_TO_CHANNEL_COMMON 1

#define CHANNEL_NORM_NEW  0
#define CHANNEL_NORM_SELF 1

/*
 * Args
 * dt   : datatype
 * x, y : channel shape
 */
channel_t *new_channel(int dt, int x, int y);
channel_t *new_channel_float32(int x, int y);

channel_t *channel_norm_float32(channel_t *ch, float32 l, float32 r, int f);

channel_t *clone_channel_float32(channel_t *ch);

/* Convert a gray image into a 2-D float32 channel, pxls val form [0, 255] to [-1, 1]*/
channel_t *gray_to_channel_float32(gray_obj *gray, int mod);

/* Convert a normalized 2-D float32 channel to gray image */
gray_obj *normalized_channel_float32_to_gray(channel_t *ch);

channel_t *free_channel(channel_t *ch);

int channel_float32_dump_to_text(channel_t *ch, const char* filename, int format);

void debug_fprint_ch_info(FILE *fp, channel_t *ch);

/* dst = dst + src */
int channel_float32_add(channel_t *dst, channel_t *src);

int channel_float32_bias(channel_t *ch, float32 bias);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_UTIL_H_ */