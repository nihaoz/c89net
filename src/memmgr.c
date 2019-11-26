#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "debug_log.h"
#include "memmgr.h"

/* support data types in data_layer.h */
#include "data_layer.h"

static list_t *global_memmgr_list = NULL;

static byte *_make_rec(char *name, void *sp, int *reclen)
{
	byte *rec = NULL;
	*reclen = (strlen(name) + 1) + sizeof(void*);
	rec = (byte*)malloc(*reclen);
	if (!rec)
		return NULL;
	strcpy((char*)rec, name);
	memcpy(rec + (strlen(name) + 1), &sp, sizeof(void*));
	return rec;
}

static bool _match_str(byte *rec, byte *any, uint len)
{
	if (strcmp((char*)rec, (char*)any))
		return false;
	return true;
}

int memmgr_init(void)
{
	if (global_memmgr_list) {
		format_log(LOG_WARN,
			"Global memory manager already initialized");
		return MEMMGR_WARN_INIT;
	}
	global_memmgr_list = list_new_dynamic(MEMMGR_INIT_REC);
	if (!global_memmgr_list)
		return MEMMGR_ERR_INIT;
	list_set_name(global_memmgr_list, "global_memmgr_list");
	return MEMMGR_STATE_OK;
}

int memmgr_clear(void)
{
	void *pdata     = NULL;
	byte *found_rec = NULL;
	int  i, type;
	if (!global_memmgr_list)
		return MEMMGR_STATE_OK;
	for (i = 0; i < global_memmgr_list->scale; ++i)
	{
		found_rec = list_get_record(global_memmgr_list, i);
		if (found_rec) {
			pdata = *((void**)
				(found_rec + (strlen((char*)found_rec) + 1)));
			sscanf((char*)found_rec, "%d", &type);
			switch (type) {
				case MEMMGR_REC_TYPE_FEATURE_MAP:
					free_feature_map(
						(feature_map_t*)pdata);
					break;
				case MEMMGR_REC_TYPE_CNN_PARA:
					free_cnn_parameters(
						(cnn_para_t*)pdata);
					break;
				default:
					break; /* Warning ...*/
			}
		}
	}
	list_del(global_memmgr_list);
	global_memmgr_list = NULL;
	return MEMMGR_STATE_OK;
}

#define __ADD_RECORD_PROC__(datatype)                                      \
	if (!((datatype*)(sp))->data->name)                                \
		return MEMMGR_ERR_NONAME;                                  \
	sprintf(fullname, "%d:%s", type,                                   \
			((datatype*)(sp))->data->name);                    \
	stat = list_search_record_hash_mod(global_memmgr_list,             \
				(byte*)fullname, strlen(fullname), 0, NULL,\
		_match_str, (byte*)fullname, strlen(fullname), &id);       \
	if (!stat)                                                         \
		return MEMMGR_ERR_EXSIT;                                   \
	stat = list_calc_hash_id(global_memmgr_list,                       \
			(byte*)fullname, strlen(fullname), 0, &id, NULL);  \
	if (stat)                                                          \
		return MEMMGR_ERR_CHASH;                                   \
	rec = _make_rec(fullname, sp, &reclen);                            \
	if (!rec)                                                          \
		return MEMMGR_ERR_ALLOC;                                   \
	list_set_record(global_memmgr_list, id, rec, reclen);              \
	free(rec);

int memmgr_add_record(int type, void *sp)
{
	char fullname[MEMMGR_REC_NAME_BUF];
	uint id;
	int  stat, reclen;
	byte *rec;
	if (!global_memmgr_list)
		return MEMMGR_ERR_NOINIT;
	switch (type) {
		case MEMMGR_REC_TYPE_FEATURE_MAP:
			__ADD_RECORD_PROC__(feature_map_t);
			return MEMMGR_STATE_OK;
		case MEMMGR_REC_TYPE_CNN_PARA:
			__ADD_RECORD_PROC__(cnn_para_t);
			return MEMMGR_STATE_OK;
		default:
			return MEMMGR_ERR_DTYPE;
	}
	return MEMMGR_STATE_OK;
}

#define __GET_RECORD_PROC__(datatype)                                \
	sprintf(fullname, "%d:%s", type, name);                      \
	stat = list_search_record_hash_mod(global_memmgr_list,       \
			(byte*)fullname, strlen(fullname), 0, NULL,  \
		_match_str, (byte*)fullname, strlen(fullname), &id); \
	if (stat)                                                    \
		return NULL;                                         \
	rec = *((void**)(list_get_record(global_memmgr_list, id) +   \
				(strlen(fullname) + 1)));

void *memmgr_get_record(int type, const char *name)
{
	char fullname[MEMMGR_REC_NAME_BUF];
	void *rec = NULL;
	uint id;
	int  stat;
	if (!global_memmgr_list)
		return NULL;
	switch (type) {
		case MEMMGR_REC_TYPE_FEATURE_MAP:
			__GET_RECORD_PROC__(feature_map_t);
			return rec;
		case MEMMGR_REC_TYPE_CNN_PARA:
			__GET_RECORD_PROC__(cnn_para_t);
			return rec;
		default:
			return NULL;
	}
	return NULL;
}

#define __DEL_RECORD_PROC__(datatype)                                  \
	sprintf(fullname, "%d:%s", type, name);                        \
	stat = list_search_record_hash_mod(global_memmgr_list,         \
			(byte*)fullname, strlen(fullname), 0, NULL,    \
		_match_str, (byte*)fullname, strlen(fullname), &id);   \
	if (stat)                                                      \
		return;                                                \
	rec = *((void**)(list_get_record(global_memmgr_list, id) +     \
					(strlen(fullname) + 1)));

void memmgr_del_record(int type, const char *name)
{
	char fullname[MEMMGR_REC_NAME_BUF];
	void *rec = NULL;
	uint id;
	int  stat;
	if (!global_memmgr_list)
		return;
	switch (type) {
		case MEMMGR_REC_TYPE_FEATURE_MAP:
			__DEL_RECORD_PROC__(feature_map_t);
			free_feature_map((feature_map_t*)rec);
			break;
		case MEMMGR_REC_TYPE_CNN_PARA:
			__DEL_RECORD_PROC__(cnn_para_t);
			free_cnn_parameters((cnn_para_t*)rec);
			break;
		default:
			return;
	}
	list_del_record(global_memmgr_list, id);
	return;
}

void debug_fprint_memmgr_info(FILE *fp)
{
	if (!global_memmgr_list) {
		format_log(LOG_INFO, "Global memory manager not initialized");
	} else {
		format_log(LOG_INFO, "global_memmgr_list info:");
		list_print_info(global_memmgr_list, fp);
	}
}

void debug_fprint_memmgr_list(FILE *fp)
{
	byte *found_rec = NULL;
	int  i, type;
	for (i = 0; i < global_memmgr_list->scale; ++i)
	{
		found_rec = list_get_record(global_memmgr_list, i);
		if (found_rec) {
			sscanf((char*)found_rec, "%d", &type);
			while (*(found_rec++) != ':');
			switch (type) {
				case MEMMGR_REC_TYPE_FEATURE_MAP:
					fprintf(fp,
						"[%04d]%s: \"%s\"\n",
						i, MM_ABBR_FEATURE_MAP,
						found_rec);
					break;
				case MEMMGR_REC_TYPE_CNN_PARA:
					fprintf(fp, "[%04d]%s: \"%s\"\n",
						i, MM_ABBR_CNN_PARA,
						found_rec);
				default:
					break; /* Warning ...*/
			}
		}
	}
}