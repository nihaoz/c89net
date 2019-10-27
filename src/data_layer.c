#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_layer.h"

data_layer *data_layer_by_channels(channel_float32 **chs, int n, const char *name)
{
	int i, ch_size = chs[0]->xsize * chs[0]->ysize;
	data_layer *l = (data_layer*)malloc(sizeof(data_layer));
	if (!l)
		return NULL;
	l->xsize = chs[0]->xsize;
	l->ysize = chs[0]->ysize;
	l->zsize = n;
	l->data = list_new_static(n, sizeof(float32) * ch_size);
	if (!l->data) {
		free(l);
		return NULL;
	}
	for (i = 0; i < n; ++i)
	{
		memcpy(l->data->mem + i * sizeof(float32) * ch_size,
				chs[i]->data, sizeof(float32) * ch_size);
	}
	list_set_name(l->data, name);
	return l;
}


data_layer *data_layer_clone(data_layer *l, const char *name)
{
	data_layer *clone = (data_layer*)malloc(sizeof(data_layer));
	if (!clone)
		return NULL;
	memcpy(clone, l, sizeof(data_layer));
	/*
	 * Since list_t *list_clone(list_t *s) Not implemented yet
	 * Replace with the code below when update lib: tlist
	 * clone->data = list_clone(l->data);
	 */
	clone->data = list_new_static(l->data->counter, l->data->blen);
	if (!clone->data)
		return NULL;
	memcpy(clone->data->mem, l->data->mem, l->data->length);
	list_set_name(clone->data, name);
	return clone;
}

channel_float32 *copy_channel_form_layer(data_layer *l, int id)
{
	channel_float32 *ch = new_channel_float32(l->xsize, l->ysize);
	int i, k_flat = l->xsize * l->ysize;
	float32 *p = (float32*)(list_get_record(l->data, id));
	for (i = 0; i < k_flat; ++i)
	{
		ch->data[i] = *(p + i);
	}
	return ch;
}

int data_layer_modify_shape(data_layer *l, int x, int y, int z)
{
	int chk = x * y * z;
	if ((l->xsize * l->ysize * l->zsize) != chk)
		return -1;
	l->xsize = x;
	l->ysize = y;
	l->zsize = z;
	l->data->blen    = x * y;
	l->data->scale   = z;
	l->data->counter = z;
	return 0;
}

data_layer *data_layer_flat(data_layer *l)
{
	int l_size = l->xsize * l->ysize;
	int data_c = l_size * l->zsize;
	data_layer *flat = (data_layer*)malloc(sizeof(data_layer));
	if (!flat)
		return NULL;
	flat->xsize = 1;
	flat->ysize = 1;
	flat->zsize = data_c;
	flat->data  = list_new_static(data_c, sizeof(float32));
	if (!flat->data) {
		free(flat);
		return NULL;
	}
	float32 *p_dst = (float32*)flat->data->mem;
	float32 *p_src = (float32*)l->data->mem;
	int i, j;
	for (i = 0; i < l_size; ++i)
	{
		for (j = 0; j < l->zsize; ++j)
		{
			*(p_dst + i * l->zsize + j) = *(p_src + j * l_size + i);
		}
	}
	return flat;
}

data_layer *free_data_layer(data_layer *l)
{
	if (l) {
		if (l->data)
			list_del(l->data);
		free(l);
	}
	return NULL;
}

para_layer *free_para_layer(para_layer *l)
{
	if (l) {
		if (l->data)
			list_del(l->data);
		free(l);
	}
	return NULL;
}

para_layer *load_conv2d_kernel_form_text(const char *filename,
				int ch_x, int ch_y, int ch_i, int ch_o, const char *name)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	int para_sum    = ch_x * ch_y * ch_i * ch_o;
	para_layer *l   = (para_layer*)malloc(sizeof(para_layer));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->type  = PARA_TYPE_KERNEL;
	l->xsize = ch_x;
	l->ysize = ch_y;
	l->zsize = ch_i;
	l->wsize = ch_o;
	l->data  = list_new_static(para_sum, sizeof(float32));
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	float32 para;
	while (fscanf(fp, "%f", &para) > 0)
		list_set_record(l->data, counter_chk++, (byte*)&para, sizeof(float32));
	if (counter_chk != para_sum) {
		fclose(fp);
		free_para_layer(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}


para_layer *load_conv2d_kernel_form_binary(const char *filename,
				int ch_x, int ch_y, int ch_i, int ch_o, const char *name)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	int para_sum    = ch_x * ch_y * ch_i * ch_o;
	para_layer *l   = (para_layer*)malloc(sizeof(para_layer));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->type  = PARA_TYPE_KERNEL;
	l->xsize = ch_x;
	l->ysize = ch_y;
	l->zsize = ch_i;
	l->wsize = ch_o;
	l->data  = list_new_static(para_sum, sizeof(float32));
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	float32 para;
	while (fread(&para, sizeof(float32), 1, fp))
		list_set_record(l->data, counter_chk++, (byte*)&para, sizeof(float32));
	if (counter_chk != para_sum) {
		fclose(fp);
		free_para_layer(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

channel_float32 *copy_kernel_form_layer(para_layer *l, int id)
{
	if (l->type != PARA_TYPE_KERNEL)
		return NULL;
	channel_float32 *ch = new_channel_float32(l->xsize, l->ysize);
	if (!ch)
		return NULL;
	int i, k_flat = l->xsize * l->ysize;
	float32 *p = (float32*)(list_get_record(l->data, k_flat * id));
	for (i = 0; i < k_flat; ++i)
	{
		ch->data[i] = *(p + i);
	}
	return ch;
}

float32 *bias_data_from_layer(para_layer *l)
{
	if (l->type != PARA_TYPE_BIAS)
		return NULL;
	float32 *p = (float32*)l->data->mem;
	return p;
}

para_layer *load_bias_form_text(const char *filename, int ch_x, const char *name)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	para_layer *l   = (para_layer*)malloc(sizeof(para_layer));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->type  = PARA_TYPE_BIAS;
	l->xsize = ch_x;
	l->data  = list_new_static(ch_x, sizeof(float32));
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	float32 para;
	while (fscanf(fp, "%f", &para) > 0)
		list_set_record(l->data, counter_chk++, (byte*)&para, sizeof(float32));
	if (counter_chk != ch_x) {
		fclose(fp);
		free_para_layer(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

para_layer *load_bias_form_binary(const char *filename, int ch_x, const char *name)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	para_layer *l   = (para_layer*)malloc(sizeof(para_layer));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->type  = PARA_TYPE_BIAS;
	l->xsize = ch_x;
	l->data  = list_new_static(ch_x, sizeof(float32));
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	float32 para;
	while (fread(&para, sizeof(float32), 1, fp))
		list_set_record(l->data, counter_chk++, (byte*)&para, sizeof(float32));
	if (counter_chk != ch_x) {
		fclose(fp);
		free_para_layer(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

void debug_fprint_data_layer_info(data_layer *l, FILE *fp)
{
	fprintf(fp, "Data layer info: \n");
	if (l->data->name)
		fprintf(fp, "Layer name: %s\n", l->data->name);
	else
		fprintf(fp, "Layer name not set !\n");
	fprintf(fp, "Size(X * Y * Z): %d * %d * %d\n", l->xsize, l->ysize, l->zsize);
	return;
}

void debug_fprint_para_layer_info(para_layer *l, FILE *fp)
{
	/*
	 * switch (l->type) {};
	 */
	fprintf(fp, "Parameters layer info: \n");
	if (l->data->name)
		fprintf(fp, "Layer name: %s\n", l->data->name);
	else
		fprintf(fp, "Layer name not set !\n");
	fprintf(fp, "Size(X * Y * Z * W): %d * %d * %d * %d\n", 
		l->xsize, l->ysize, l->zsize, l->wsize);
	return;
}
