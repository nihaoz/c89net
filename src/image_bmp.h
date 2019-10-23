#ifndef _IMAGE_BMP_H_
#define _IMAGE_BMP_H_

#ifdef __cplusplus
	extern "C" {
#endif

#define BMP_HEADER 54

#include "image_util.h"

/* Specify a bmp file, returns file size and get x-size,
 * y-size of the bmp file, return 0 if cannot open bmp file.
 */
int img_get_bmp_size(const char *filename, int *xsize,int *ysize);

/* Read a bmp file, and return the rgb_obj */
rgb_obj *img_read_bmp(const char *filename);

/* Save rgb image object to disk */
int img_save_rgb_bmp(rgb_obj *rgb, const char *filename);

/* Save gray image object as 24-bit bmp to disk */
int img_save_gray_as_rgb_bmp(gray_obj *gray, const char *filename);

/* Get a pixel from bmp object, return -1 if failed */
int img_get_pixel(rgb_obj *bmp, int x, int y, rgb_pixel_t *p);

/* Write a pixel to bmp object, return -1 if failed */
int img_write_pixel(rgb_obj *bmp, int x, int y, rgb_pixel_t *p);

/* Not suggested to use */
int _get_pixel(rgb_pixel_t *pixel,
				byte *img,
				int xsize, int ysize,
				int x, int y);
int _write_pixel(rgb_pixel_t *pixel,
				byte *img,
				int xsize, int ysize,
				int x, int y);
int _bmp_write(byte *image, int xsize, int ysize, const char *filename);
int _bmp_read(byte *image, int xsize, int ysize, const char *filename);

#ifdef __cplusplus
	}
#endif

#endif /* _IMAGE_BMP_H_ */