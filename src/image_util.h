#ifndef _IMAGE_UTIL_H_
#define _IMAGE_UTIL_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef byte
	#define byte unsigned char
#endif

#define GRAY_COLOR_BLANK 255
#define RGB_COLOR_BLANK  255

#ifndef byte
	#define byte unsigned char
#endif

/* #define PXL_FORMAT_OCT */
#define PXL_FORMAT_DEF 0
#define PXL_FORMAT_DEC 0
#define PXL_FORMAT_HEX 1

typedef struct {
	byte r;
	byte g;
	byte b;
} rgb_pixel_t;

typedef struct
{
	byte *data;
	int xsize;
	int ysize;
} gray_obj;

typedef struct
{
	byte *data;
	int xsize;
	int ysize;
} rgb_obj;

/* Create a gray object */
gray_obj *img_new_gray(int xsize, int ysize, int init);

/* Create a rgb image object */
rgb_obj *img_new_rgb(int xsize, int ysize, int init);

/* Delete a rgb_obj and release memory */
rgb_obj *img_free_rgb(rgb_obj *rgb);

/* Delete a gray_obj and release memory */
gray_obj *img_free_gray(gray_obj *gray);

/* Convert a RGB-pixel to gray-pixel, and return the gray scale */
byte img_pixel_switch_2_gray(rgb_pixel_t *pixel);

/* 
 * Assume a 24-bit bmp image was loaded in memory, pointer 'image'
 * contains the pixels of the image, each pixel has RGB-channel included.
 * 'rgb_split' aquires the pointer 'image', and splits it into 3 channels
 * 'r', 'g' and 'b' for red, green and blue channel of the image.
 */

void img_rgb_split(byte *image, int size, byte *r, byte *g, byte *b);
/*
 * 'rgb_mix' take 3 channels of red green and blue from pointers 'r', 'g'
 * and 'b' and mixes them up to a 24-bit bmp image pixel sequence 'image'.
 */
void img_rgb_mix(byte *image, int size, byte *r, byte *g, byte *b);

/* Calculations of address and coordinate rgb_rgb_pixel_t*/
int img_iaddress_calc(int xsize, int ysize, int x,int y);
int img_coordinate_calc(int iaddress, int xsize, int ysize, int *x, int *y);

/* Dump image as text to file */
int dump_rgb_to_text(rgb_obj *rgb, const char* filename, int format);
int dump_gray_to_text(gray_obj* gray, const char* filename, int format);

#ifdef __cplusplus
	}
#endif

#endif /* _IMAGE_UTIL_H_ */