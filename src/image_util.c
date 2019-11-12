#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_util.h"

const char _pxl_print_format[2][6] = {
	{"%03d "},
	{"%02d "}
};

gray_obj *img_new_gray(int xsize, int ysize, int init)
{
	gray_obj *gray = (gray_obj*)malloc(sizeof(gray_obj));
	if (!gray)
		return NULL;
	gray->xsize = xsize;
	gray->ysize = ysize;
	gray->data = (byte*)malloc(xsize * ysize);
	if (!gray->data) {
		free(gray);
		return NULL;
	}
	if (init != -1)
		memset(gray->data, init, xsize * ysize);
	return gray;
}

rgb_obj *img_new_rgb(int xsize, int ysize, int init)
{
	rgb_obj *rgb = (rgb_obj*)malloc(sizeof(rgb_obj));
	if (!rgb)
		return NULL;
	rgb->xsize = xsize;
	rgb->ysize = ysize;
	rgb->data = (byte*)malloc(xsize * ysize * 3);
	if (!rgb->data) {
		free(rgb);
		return NULL;
	}
	if (init != -1)
		memset(rgb->data, init, xsize * ysize * 3);
	return rgb;
}

gray_obj *img_free_gray(gray_obj *gray)
{
	if (gray) {
		if (gray->data)
			free(gray->data);
		free(gray);
	}
	return NULL;
}

rgb_obj *img_free_rgb(rgb_obj *rgb)
{
	if (rgb) {
		if (rgb->data)
			free(rgb->data);
		free(rgb);
	}
	return NULL;
}

byte img_pixel_switch_2_gray(rgb_pixel_t *pixel)
{
	/*
	byte pv = 
		0.3 * pixel->r + 0.59 * pixel->g + 0.11 * pixel->b;
	*/
	byte pv = (pixel->r + pixel->g + pixel->b) / 3;
	pixel->r = pv;
	pixel->g = pv;
	pixel->b = pv;
	return pv;
}

void img_rgb_split(byte *image, int size, byte *r, byte *g, byte *b)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		*b++ = *(image + i * 3);
		*g++ = *(image + i * 3 + 1);
		*r++ = *(image + i * 3 + 2);
	}
	return;
}

void img_rgb_mix(byte *image, int size, byte *r, byte *g, byte *b)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		*(image + i * 3) = *b++;
		*(image + i * 3 + 1) = *g++;
		*(image + i * 3 + 2) = *r++;
	}
	return;
}

int img_iaddress_calc(int xsize,int ysize,int x,int y)
{
	int pixel_max = xsize * ysize;
	int pixel_p = xsize * y + x;
	if (pixel_p > pixel_max)
		return -1;
	return pixel_p;
}

int img_coordinate_calc(int iaddress, int xsize, int ysize,int *x,int *y)
{
	int pixel_max = xsize * ysize;
	if(iaddress > pixel_max)
		return -1;
	*x = iaddress % xsize;
	*y = iaddress / xsize;
	return 0;
}


int dump_rgb_to_text(rgb_obj *rgb, const char* filename, int format)
{
	FILE *fp = fopen(filename, "w+");
	if (!fp)
		return -1;
	int i, j;
	for (i = 0; i < rgb->ysize; ++i)
	{
		for (j = 0; j < rgb->xsize; ++j)
		{
			fprintf(fp, _pxl_print_format[format],
				*(rgb->data + i * rgb->xsize + j + 0));
			fprintf(fp, _pxl_print_format[format],
				*(rgb->data + i * rgb->xsize + j + 1));
			fprintf(fp, _pxl_print_format[format],
				*(rgb->data + i * rgb->xsize + j + 2));
		}
	}
	fclose(fp);
	return 0;
}

int dump_gray_to_text(gray_obj* gray, const char* filename, int format)
{
	FILE *fp = fopen(filename, "w+");
	if (!fp)
		return -1;
	int i, j;
	for (i = 0; i < gray->ysize; ++i)
	{
		for (j = 0; j < gray->xsize; ++j)
		{
			fprintf(fp, _pxl_print_format[format],
				*(gray->data + i * gray->xsize + j));
		}
	}
	fclose(fp);
	return 0;
}
