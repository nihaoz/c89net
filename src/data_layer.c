#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_layer.h"

feature_map_t *feature_map_by_channels(channel_t **chs, int n, const char *name)
{
	feature_map_t *l = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!l)
		return NULL;
	int i, ch_size = chs[0]->xsize * chs[0]->ysize;
	l->datatype = chs[0]->datatype;
	l->xsize = chs[0]->xsize;
	l->ysize = chs[0]->ysize;
	l->zsize = n;
	l->data = list_new_static(n, sizeof_datatype(l->datatype) * ch_size);
	if (!l->data) {
		free(l);
		return NULL;
	}
	for (i = 0; i < n; ++i)
	{
		memcpy(l->data->mem + i * sizeof_datatype(l->datatype) * ch_size,
				chs[i]->data,  sizeof_datatype(l->datatype) * ch_size);
	}
	list_set_name(l->data, name);
	return l;
}


feature_map_t *feature_map_clone(feature_map_t *l, const char *name)
{
	feature_map_t *clone = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!clone)
		return NULL;
	memcpy(clone, l, sizeof(feature_map_t));
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

channel_t *copy_channel_form_layer(feature_map_t *l, int id)
{
	channel_t *ch = new_channel(l->datatype, l->xsize, l->ysize);
	int i, k_flat = l->xsize * l->ysize;
	void *p = (void*)(list_get_record(l->data, id));
	memcpy(ch->data, p, sizeof_datatype(l->datatype) * k_flat);
	return ch;
}

int feature_map_modify_shape(feature_map_t *l, int x, int y, int z)
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

feature_map_t *feature_map_flat(feature_map_t *l)
{
	int l_size = l->xsize * l->ysize;
	int data_c = l_size * l->zsize;
	feature_map_t *flat = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!flat)
		return NULL;
	flat->xsize = 1;
	flat->ysize = 1;
	flat->zsize = data_c;
	flat->data  = list_new_static(data_c, sizeof(sizeof_datatype(l->datatype)));
	if (!flat->data) {
		free(flat);
		return NULL;
	}
	byte *p_dst = (byte*)flat->data->mem;
	byte *p_src = (byte*)l->data->mem;
	int i, j, unit_len = sizeof_datatype(l->datatype);
	for (i = 0; i < l_size; ++i)
	{
		for (j = 0; j < l->zsize; ++j)
		{
			memcpy((p_dst + (i * l->zsize + j) * unit_len),
				(p_src + (j * l_size + i) * unit_len), unit_len);
		}
	}
	return flat;
}

feature_map_t *free_feature_map(feature_map_t *l)
{
	if (l) {
		if (l->data)
			list_del(l->data);
		free(l);
	}
	return NULL;
}

cnn_para_t *free_cnn_parameters(cnn_para_t *l)
{
	if (l) {
		if (l->data)
			list_del(l->data);
		free(l);
	}
	return NULL;
}

cnn_para_t *load_cnn_conv2d_kernel(const char *filename,
				int ch_x, int ch_y, int ch_i, int ch_o, const char *name)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	int para_sum    = ch_x * ch_y * ch_i * ch_o;
	cnn_para_t *l   = (cnn_para_t*)malloc(sizeof(cnn_para_t));
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
		free_cnn_parameters(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

channel_t *copy_kernel_form_layer(cnn_para_t *l, int id)
{
	if (l->type != PARA_TYPE_KERNEL)
		return NULL;
	channel_t *ch = new_channel(l->datatype, l->xsize, l->ysize);
	if (!ch)
		return NULL;
	int i, k_flat = l->xsize * l->ysize;
	void *p = (void*)(list_get_record(l->data, k_flat * id));
	memcpy(ch->data, p, sizeof_datatype(l->datatype) * k_flat);
	return ch;
}

float32 *bias_from_cnn_parameters(cnn_para_t *l)
{
	if (l->type != PARA_TYPE_BIAS)
		return NULL;
	float32 *p = (float32*)l->data->mem;
	return p;
}

cnn_para_t *load_cnn_bias(const char *filename, int ch_x, const char *name)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	int counter_chk = 0;
	cnn_para_t *l   = (cnn_para_t*)malloc(sizeof(cnn_para_t));
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
		free_cnn_parameters(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

void debug_fprint_feature_map_info(feature_map_t *l, FILE *fp)
{
	fprintf(fp, "Data layer info: \n");
	if (l->data->name)
		fprintf(fp, "Layer name: %s\n", l->data->name);
	else
		fprintf(fp, "Layer name not set !\n");
	fprintf(fp, "Size(X * Y * Z): %d * %d * %d\n", l->xsize, l->ysize, l->zsize);
	return;
}

void debug_fprint_cnn_parameters_info(cnn_para_t *l, FILE *fp)
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
