#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_bmp.h"

int img_get_bmp_size(const char *filename, int *xsize, int *ysize)
{
	byte header[54];
	unsigned int *x;
	FILE *fp;
	fp = fopen(filename, "rb");
	if (fp == NULL)
		return 0;
	fread(header, sizeof(byte), BMP_HEADER, fp);
	fclose(fp);
	x = (unsigned int*)(header + 18);
	*xsize = x[0];
	*ysize = x[1];
	return *xsize * *ysize;
}

int _bmp_read(byte *image, int xsize, int ysize, const char *filename)
{
	byte header[BMP_HEADER];
	byte *hp = header;
	unsigned int b;
	int windows_byte_cnt = 0;
	int r = (xsize * 3 ) % 4;
	byte *image_tail = NULL;
	FILE *fp;
	if (!(fp = fopen(filename, "rb"))) 
		return -1;
	fread(header, sizeof(byte), BMP_HEADER, fp);
	b = *(unsigned int*)(hp + 10);
	fseek(fp, b, SEEK_SET);
	if (!r) {
		windows_byte_cnt = 0;
	} else {
		int t = (xsize * 3 ) / 4;
		windows_byte_cnt = ((t + 1) * 4) - xsize * 3;
	}
	image_tail = image + (ysize * xsize * 3);
	/* fread(image, sizeof(byte), xsize * ysize * 3, fp); */
	for (r = 0; r < ysize; ++r)
	{
		image_tail -= (xsize * 3);
		fread(image_tail , sizeof(byte), xsize * 3, fp);
		fseek(fp, windows_byte_cnt, SEEK_CUR);
	}
	fclose(fp);
	return 0;
}

rgb_obj *img_read_bmp(const char *filename)
{
	int x, y, s;
	rgb_obj *bmp = (rgb_obj*)malloc(sizeof(rgb_obj));
	if (!bmp)
		return NULL;
	s = img_get_bmp_size(filename, &x, &y);
	if (!s) {
		free(bmp);
		return NULL;
	}
	bmp->xsize = x;
	bmp->ysize = y;
	bmp->data = (byte*)malloc(s * 3);
	if (!bmp->data) {
		free(bmp);
		return NULL;
	}
	_bmp_read(bmp->data, x, y, filename);
	return bmp;
}

int img_save_rgb_bmp(rgb_obj *rgb, const char *filename)
{
	return _bmp_write(rgb->data, rgb->xsize, rgb->ysize, filename);
}

int img_save_gray_as_rgb_bmp(gray_obj *gray, const char *filename)
{
	rgb_obj *tmp = img_new_rgb(gray->xsize, gray->ysize, 0);

	img_rgb_mix(tmp->data, gray->xsize * gray->ysize,
					gray->data, gray->data, gray->data);
	img_save_rgb_bmp(tmp, filename);
	img_free_rgb(tmp);
	return 0;
}

int _bmp_write(byte *image, int xsize, int ysize, const char *filename)
{
	byte header[BMP_HEADER] = {
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0
	};
	int windows_byte_cnt = 0;
	int windows_byte_set = 0;
	int r = (xsize * 3 ) % 4;
	unsigned int *x;
	byte *image_tail = NULL;
	FILE *fp;
	if (!r) {
		windows_byte_cnt = 0;
	} else {
		int t = (xsize * 3 ) / 4;
		windows_byte_cnt = ((t + 1) * 4) - xsize * 3;
	}
	x = (unsigned int*)(header + 2);
	x[0] = (xsize * 3 + windows_byte_cnt) * ysize + BMP_HEADER;
	x = (unsigned int*)(header + 18);
	x[0]=xsize;
	x[1]=ysize;
	if (!(fp = fopen(filename, "wb+")))
		return -1;
	fwrite(header, sizeof(byte), BMP_HEADER, fp);
	/* fwrite(image, sizeof(byte) * 3, xsize * ysize, fp); */
	image_tail = image + (ysize * xsize * 3);
	for (r = 0; r < ysize; ++r)
	{
		image_tail -= (xsize * 3);
		fwrite(image_tail, sizeof(byte) * 3, xsize, fp);
		fwrite(&windows_byte_set, 1, windows_byte_cnt, fp);
	}
	fclose(fp);
	return 0;
}

int img_get_pixel(rgb_obj *bmp, int x, int y, rgb_pixel_t *p)
{
	return _get_pixel(p, bmp->data, bmp->xsize, bmp->ysize, x, y);
}

int _get_pixel(rgb_pixel_t *pixel,
			byte *img,
			int xsize, int ysize,
			int x, int y)
{
	int pixel_max = xsize * ysize;
	int pixel_p = xsize * y + x;
	if (pixel_p > pixel_max)
		return -1;
	pixel_p *= 3;
	pixel->b = img[pixel_p];
	pixel->g = img[pixel_p + 1];
	pixel->r = img[pixel_p + 2];
	return pixel_p;
}

int img_write_pixel(rgb_obj *bmp, int x, int y, rgb_pixel_t *p)
{
	return _write_pixel(p, bmp->data, bmp->xsize, bmp->ysize, x, y);
}

int _write_pixel(rgb_pixel_t *pixel,
			byte *img,
			int xsize, int ysize,
			int x, int y)
{
	int pixel_max = xsize * ysize;
	int pixel_p = xsize * y + x;	
	if (pixel_p > pixel_max)
		return -1;
	pixel_p *= 3;
	img[pixel_p] = pixel->b;
	img[pixel_p + 1] = pixel->g;
	img[pixel_p + 2] = pixel->r;
	return pixel_p;
}
