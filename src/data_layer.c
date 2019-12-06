#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_MEMMGR
	#include "memmgr.h"
#endif
#include "debug_log.h"
#include "data_layer.h"

feature_map_t *feature_map_by_channels(channel_t **chs,
				int n, const char *name)
{
	int i, ch_size;
	feature_map_t *l = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!l) {
		QUICK_LOG_ERR_MEM_ALLOC(l);
		return NULL;
	}
	ch_size     = chs[0]->xsize * chs[0]->ysize;
	l->datatype = chs[0]->datatype;
	l->xsize    = chs[0]->xsize;
	l->ysize    = chs[0]->ysize;
	l->zsize    = n;
	l->data     = list_new_static(l->zsize,
			sizeof_datatype(l->datatype) * ch_size);
	if (!l->data) {
		free(l);
		QUICK_LOG_ERR_MEM_ALLOC(l->data);
		return NULL;
	}
	for (i = 0; i < n; ++i)
	{
		memcpy(l->data->mem + 
			i * sizeof_datatype(l->datatype) * ch_size,
		chs[i]->data,  sizeof_datatype(l->datatype) * ch_size);
	}
	list_set_name(l->data, name);
	return l;
}

feature_map_t *feature_map_clone(feature_map_t *l, const char *name)
{
	feature_map_t *clone = (feature_map_t*)malloc(sizeof(feature_map_t));
	if (!clone) {
		QUICK_LOG_ERR_MEM_ALLOC(clone);
		return NULL;
	}
	memcpy(clone, l, sizeof(feature_map_t));
	/*
	 * Since list_t *list_clone(list_t *s) Not implemented yet
	 * Replace with the code below when update lib: tlist
	 * clone->data = list_clone(l->data);
	 */
	clone->data = list_new_static(l->data->counter, l->data->blen);
	if (!clone->data) {
		free(clone);
		QUICK_LOG_ERR_MEM_ALLOC(clone->data);
		return NULL;
	}
	memcpy(clone->data->mem, l->data->mem, l->data->length);
	list_set_name(clone->data, name);
	return clone;
}

channel_t *copy_channel_form_layer(feature_map_t *l, int id)
{
	channel_t *ch = new_channel(l->datatype, l->xsize, l->ysize);
	int k_flat = l->xsize * l->ysize;
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

feature_map_t *feature_map_flat(feature_map_t *l, const char *name)
{
	int l_size = l->xsize * l->ysize;
	int data_c = l_size * l->zsize;
	int i, j, unit_len = sizeof_datatype(l->datatype);
	byte *p_dst, *p_src;
#ifdef ENABLE_MEMMGR
	feature_map_t *flat =
		(feature_map_t*)
		memmgr_get_record(MEMMGR_REC_TYPE_FEATURE_MAP, name);
#else
	feature_map_t *flat = NULL;
#endif
	if (!flat) {
		flat = (feature_map_t*)malloc(sizeof(feature_map_t));
		if (!flat) {
			QUICK_LOG_ERR_MEM_ALLOC(flat);
			return NULL;
		}
		flat->datatype = l->datatype;
		flat->xsize = 1;
		flat->ysize = 1;
		flat->zsize = data_c;
		flat->data  = list_new_static(data_c,
				sizeof(sizeof_datatype(l->datatype)));
		if (!flat->data) {
			free(flat);
			QUICK_LOG_ERR_MEM_ALLOC(flat->data);
			return NULL;
		}
		list_set_name(flat->data, name);
#ifdef ENABLE_MEMMGR
		memmgr_add_record(MEMMGR_REC_TYPE_FEATURE_MAP, flat);
#endif
	}
	p_dst = (byte*)flat->data->mem;
	p_src = (byte*)l->data->mem;
	for (i = 0; i < l_size; ++i)
	{
		for (j = 0; j < l->zsize; ++j)
		{
			memcpy((p_dst + (i * l->zsize + j) * unit_len),
				(p_src + (j * l_size + i) * unit_len),
				unit_len);
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
	int counter_chk = 0;
	int para_sum    = ch_x * ch_y * ch_i * ch_o;
	cnn_para_t *l = NULL;
	float32 para;
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	l = (cnn_para_t*)malloc(sizeof(cnn_para_t));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->datatype = DATATYPE_FLOAT32;
	l->type     = PARA_TYPE_KERNEL;
	l->xsize    = ch_x;
	l->ysize    = ch_y;
	l->zsize    = ch_i;
	l->wsize    = ch_o;
	l->data     = list_new_static(para_sum, sizeof(float32));
	format_log(LOG_WARN,"load_cnn_conv2d_kernel can only load little-endian float32 stream file %s: %d",\
						 __FILE__, __LINE__);
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	while (fread(&para, sizeof(float32), 1, fp))
		list_set_record(l->data, counter_chk++,
			(byte*)&para, sizeof(float32));
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
	int k_flat;
	channel_t *ch;
	if (l->type != PARA_TYPE_KERNEL)
		return NULL;
	ch = new_channel(l->datatype, l->xsize, l->ysize);
	if (!ch)
		return NULL;
	k_flat = l->xsize * l->ysize;
	memcpy(ch->data, (list_get_record(l->data, k_flat * id)),
		sizeof_datatype(l->datatype) * k_flat);
	return ch;
}

float32 *bias_from_cnn_parameters(cnn_para_t *l)
{
	if (l->type != PARA_TYPE_BIAS)
		return NULL;
	return (float32*)l->data->mem;
}

cnn_para_t *load_cnn_bias(const char *filename,
	int ch_i, const char *name)
{
	int counter_chk = 0;
	float32 para;
	cnn_para_t *l = NULL;
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	l = (cnn_para_t*)malloc(sizeof(cnn_para_t));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->datatype = DATATYPE_FLOAT32;
	l->type     = PARA_TYPE_BIAS;
	l->xsize    = 1;
	l->ysize    = 1;
	l->zsize    = ch_i;
	l->data     = list_new_static(ch_i, sizeof(float32));
	format_log(LOG_WARN,"load_cnn_bias can only load little-endian float32 stream file %s: %d",\
						 __FILE__, __LINE__);
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	while (fread(&para, sizeof(float32), 1, fp))
		list_set_record(l->data, counter_chk++,
			(byte*)&para, sizeof(float32));
	if (counter_chk != ch_i) {
		fclose(fp);
		free_cnn_parameters(l);
		return NULL;
	}
	fclose(fp);
	list_set_name(l->data, name);
	return l;
}

cnn_para_t *load_cnn_batch_norm(const char *filename,
	int ch_i, const char *name)
{
	int counter_chk = 0;
	int para_sum = PARA_BN_CHK * ch_i;
	float32 para;
	cnn_para_t *l = NULL;
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	l = (cnn_para_t*)malloc(sizeof(cnn_para_t));
	if (!l) {
		fclose(fp);
		return NULL;
	}
	l->datatype  = DATATYPE_FLOAT32;
	l->type      = PARA_TYPE_BN;
	l->xsize     = PARA_BN_CHK;
	l->ysize     = 1;
	l->zsize     = ch_i;
	l->data      = list_new_static(para_sum, sizeof(float32));
	format_log(LOG_WARN,"load_cnn_batch_norm can only load little-endian float32 stream file %s: %d",\
						 __FILE__, __LINE__);
	if (!l->data) {
		fclose(fp);
		free(l);
		return NULL;
	}
	while (fread(&para, sizeof(float32), 1, fp))
		list_set_record(l->data, counter_chk++,
			(byte*)&para, sizeof(float32));
	if (counter_chk != para_sum) {
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
	fprintf(fp, "Size(X Y Z): %d %d %d\n",
		l->xsize, l->ysize, l->zsize);
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
	fprintf(fp, "Size(X Y Z W): %d %d %d %d\n", 
		l->xsize, l->ysize, l->zsize, l->wsize);
	return;
}
