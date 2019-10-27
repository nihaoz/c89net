#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#ifdef ENABLE_SSHM
	#include "sshm.h"
#endif
#ifdef ENABLE_ZLIB
	/* #include <gzguts.h> */
	#include <zlib.h>
#endif /* ENABLE_ZLIB */

#define BUILD_BUG_ON(condition)\
		((void)sizeof(char[1 - 2*!!(condition)]))
/*=========== compile-time test ===========*/
void __________compile_time_test___________()
{
	/* LIST_INFO_LEN) must be 32 */
	BUILD_BUG_ON(32 - LIST_INFO_LEN);
}


static uint _djb_hash(byte *s, uint len, uint seed)
{
	unsigned int hash = seed;
	uint i;
	for (i = 0; i < len; i++) {
		hash = (hash << 5) + hash + s[i];
	}
	return hash;
}

static bool _is_little_endian()
{
	unsigned int i = 1;
	unsigned char *c = (unsigned char*)&i;
	if (*c)
		return true;
	return false;
}

static void _memswap(byte *p1, byte *p2, uint len)
{
	while (len--) {
		*(p1) ^= *(p2);
		*(p2) ^= *(p1);
		*(p1++) ^= *(p2++);
	}
}

void _memrev(void *i, int s)
{
	int j = s >> 1;
	byte *c = (byte*)i;
	for (s--; j > 0; j--) {
		*c ^= *(c + s);
		*(c + s) ^= *c;
		*c ^= *(c + s);
		c++, s -= 2;
	}
}

list_t *list_new_static(uint scale, uint blen)
{
	if (!scale)
		return NULL;
	if (!blen)
		return NULL;
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if (!list)
		return NULL;
	memset(list, 0, sizeof(list_t));
	SET_FLAG(list->flag, LIST_STATIC_MODE);
	list->length = scale * blen;
	list->mem = (byte*)malloc(list->length);
	if (!list->mem)
		list->status |= LIST_MALLOC_ERROR;
	else
		memset(list->mem, 0, list->length);
	list->blen = blen;
	list->scale = scale;
	list->counter = scale;
	return list;
}

list_t *list_new_dynamic(uint scale)
{
	if (!scale)
		return NULL;
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if (!list)
		return NULL;
	memset(list, 0, sizeof(list_t));
	SET_FLAG(list->flag, LIST_DYNAMIC_MODE);
	list->index = (byte**)malloc(scale * sizeof(byte*));
	if (!list->index)
		list->status |= LIST_MALLOC_ERROR;
	else
		memset(list->index, 0, scale * sizeof(byte*));
	list->scale = scale;
	return list;
}

__status list_del(list_t *list)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_del_dynamic(list);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_del_static(list);
	else
		return OPS_BAD_OBJ;
}

__status list_del_static(list_t *list)
{
	if (!list)
		return OPS_SUCCESS;
	if (list->name)
		free(list->name);
	if (list->mem)
		free(list->mem);
	free(list);
	return OPS_SUCCESS;
}

__status list_del_dynamic(list_t *list)
{
	if (!list)
		return OPS_SUCCESS;
	if (list->name)
		free(list->name);
	while (list->scale--) {
		if (list->index[list->scale])
			free(list->index[list->scale]);
	}
	if (list->index)
		free(list->index);
	free(list);
	return OPS_SUCCESS;
}

__status list_set_name(list_t *list, const char *name)
{
	if (TEST_FLAG(list->flag, LIST_NOT_SHARED)) {
		if (!name) {
			if (list->name)
				free(list->name);
			list->name = NULL;
			return OPS_SUCCESS;
		}
		if (strlen(name) >= LIST_NAME_LEN)
			return OPS_ARG_ILL;
		if (list->name)
			free(list->name);
		list->name = (char*)malloc(LIST_NAME_LEN);
		if (!list->name)
			return OPS_MALLOC_ERR;
		strcpy(list->name, name);
	}
	else {
		memset((char*)list + sizeof(list_t), 0, LIST_NAME_LEN);
		strcpy((char*)list + sizeof(list_t), name);
	}
	return OPS_SUCCESS;
}


__status list_resize(list_t *list, uint newScale)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_resize_dynamic(list, newScale);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_resize_static(list, newScale);
	else
		return OPS_BAD_OBJ;
}

__status list_resize_static(list_t *list, uint newScale)
{
	if (TEST_FLAG(list->flag, LIST_UNRESIZABLE))
		return OPS_BAD_OBJ;
	ulong newLen = newScale * list->blen;
	byte *newMem = (byte*)realloc(list->mem, newLen);
	if (!newMem)
		return OPS_MALLOC_ERR;
	if (newScale > list->scale) {
		memset(list->mem + list->scale * list->blen, 0,
			(newScale - list->scale) * list->blen);
	}
	list->mem = newMem;
	list->length = newLen;
	list->scale = newScale;
	list->counter = newScale;
	return OPS_SUCCESS;
}

__status list_resize_dynamic(list_t *list, uint newScale)
{
	if (TEST_FLAG(list->flag, LIST_UNRESIZABLE))
		return OPS_BAD_OBJ;
	if (newScale < list->scale) {
		uint i = newScale;
		for (; i < list->scale; ++i)
			list_del_dynamic_record(list, i);
	}
	byte **newIndex = (byte**)realloc(list->index,
					newScale * sizeof(byte*));
	if (!newIndex)
		return OPS_MALLOC_ERR;
	list->index = newIndex;
	if (newScale > list->scale) {
		memset((byte*)newIndex + list->scale * sizeof(byte*), 0,
			(newScale - list->scale) * sizeof(byte*));
	}
	list->scale = newScale;
	return OPS_SUCCESS;
}

__status list_set_record(list_t *list, uint id,
	byte *record, uint len)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_set_dynamic_record(list, id, record, len);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_set_static_record(list, id, record, len);
	else
		return OPS_BAD_OBJ;
}

__status list_set_static_record(list_t *list, uint id,
	byte *record, uint len)
{
	if (id >= list->scale)
		return OPS_BAD_ID;
	len = list->blen < len ? list->blen : len;
	memcpy(list->mem + id * list->blen, record, len);
	return OPS_SUCCESS;
}

__status list_set_dynamic_record(list_t *list, uint id,
	byte *record, uint len)
{
	if (id >= list->scale)
		return OPS_BAD_ID;
	if (list->index[id])
		return OPS_DYN_ID_EXIST;
	uint mlen = len + sizeof(DynLenFlag);
	list->index[id] = (byte*)malloc(mlen);
	if (!list->index[id])
		return OPS_MALLOC_ERR;
	memcpy(list->index[id] + sizeof(DynLenFlag), record, len);
	DynLenFlag *l = (DynLenFlag*)list->index[id];
	*l = len;
	list->counter++;
	list->length += mlen;
	return OPS_SUCCESS;
}

bool list_test_record(list_t *list, uint id, uint offset,
	uint nbyte)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		if (list->index[id])
			return true;
		else
			return false;
	} else if (TEST_FLAG(list->flag, LIST_STATIC_MODE)) {
		uint c = 0;
		byte *rec = list_get_static_record(list, id);
		while (nbyte--) {
			c += rec[offset + nbyte];
		}
		if (c)
			return true;
		else
			return false;
	}
	else
		return false;
}

byte *list_get_record(list_t *list, uint id)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_get_dynamic_record(list, id);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_get_static_record(list, id);
	else
		return NULL;
}

byte *list_get_static_record(list_t *list, uint id)
{
	if (id >= list->scale)
		return NULL;
	else
		return list->mem + id * list->blen;
}

byte *list_get_dynamic_record(list_t *list, uint id)
{
	if (id >= list->scale)
		return NULL;
	else {
		if (list->index[id])
			return (list->index[id] + sizeof(DynLenFlag));
		else
			return NULL;
	}
}

uint list_get_record_len(list_t *list, uint id)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		byte *rec = list_get_dynamic_record(list, id);
		if (rec)
			return list_get_dynamic_record_len(rec);
		else
			return 0;
	} else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list->blen;
	else
		return 0;	
}

__status list_del_record(list_t *list, uint id)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_del_dynamic_record(list, id);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_del_static_record(list, id);
	else
		return OPS_BAD_OBJ;
}

__status list_del_static_record(list_t *list, uint id)
{
	if (id >= list->scale)
		return OPS_BAD_ID;
	memset(list->mem + id * list->blen, 0, list->blen);
	return OPS_SUCCESS;
}

__status list_del_dynamic_record(list_t *list, uint id)
{
	if (id >= list->scale)
		return OPS_BAD_ID;
	if (list->index[id]) {
		list->length -= sizeof(DynLenFlag);
		list->length -=
			list_get_dynamic_record_len(list->index[id]);
		list->counter--;
		free(list->index[id]);
		list->index[id] = NULL;
	}
	return OPS_SUCCESS;
}

__status list_swap_record(list_t *list,
	uint id1, uint id2)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_swap_dynamic_record(list, id1, id2);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_swap_static_record(list, id1, id2);
	else
		return OPS_BAD_OBJ;
}

__status list_swap_static_record(list_t *list,
	uint id1, uint id2)
{
	if (id1 >= list->scale || id2 >= list->scale)
		return OPS_BAD_ID;
	_memswap(list->mem + id1 * list->blen,
		list->mem + id2 * list->blen, list->blen);
	return OPS_SUCCESS;	
}

__status list_swap_dynamic_record(list_t *list,
	uint id1, uint id2)
{
	if (id1 >= list->scale || id2 >= list->scale)
		return OPS_BAD_ID;
	byte* p;
	p = list->index[id1];
	list->index[id1] = list->index[id2];
	list->index[id2] = p;
	return OPS_SUCCESS;
}

static void _switch_byte_order(list_t *list) {
	_memrev(&list->length, sizeof(ulong));
	_memrev(&list->key, sizeof(uint));
	_memrev(&list->counter, sizeof(uint));
	_memrev(&list->blen, sizeof(uint));
	_memrev(&list->scale, sizeof(uint));
}

__status list_export(list_t *list, const char *path,
	const char *mode)
{
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE))
		return list_export_dynamic(list, path, mode);
	else if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		return list_export_static(list, path, mode);
	else
		return OPS_BAD_OBJ;
}

__status list_export_static(list_t *list, const char *path,
	const char *mode)
{
	IO_FILE fp = NULL;
	if (mode)
		fp = IO_fopen(path, mode);
	else
		fp = IO_fopen(path, LIST_DEF_EXPORT_FILE_MODE);
	if (!fp)
		return OPS_FILE_ERR;
	uint name_len = 0;
	if (list->name)
		name_len = strlen(list->name);
	if (!_is_little_endian()) {
		_memrev(&name_len, sizeof(uint));
		_switch_byte_order(list);
	}
	if (IO_fwrite(&name_len, sizeof(uint), 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	if (IO_fwrite(list->name, name_len, 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	if (IO_fwrite((byte*)list + LIST_INFO_OFFSET,
			       	LIST_INFO_LEN, 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	if (IO_fwrite(list->mem, list->length, 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	IO_fclose(fp);
	if (!_is_little_endian()) {
		_switch_byte_order(list);
	}
	return OPS_SUCCESS;
}

__status list_export_dynamic(list_t *list, const char *path,
	const char *mode)
{
	IO_FILE fp = NULL;
	if (mode)
		fp = IO_fopen(path, mode);
	else
		fp = IO_fopen(path, LIST_DEF_EXPORT_FILE_MODE);
	if (!fp)
		return OPS_FILE_ERR;
	uint name_len = 0;
	if (list->name)
		name_len = strlen(list->name);
	if (!_is_little_endian()) {
		_memrev(&name_len, sizeof(uint));
		_switch_byte_order(list);
	}
	if (IO_fwrite(&name_len, sizeof(uint), 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	if (IO_fwrite(list->name, name_len, 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	if (IO_fwrite((byte*)list + LIST_INFO_OFFSET,
			       	LIST_INFO_LEN, 1, fp) < 0) {
		IO_fclose(fp);
		return OPS_FILE_ERR;
	}
	uint sc = list->scale;
	while (sc--) {
		if (list->index[sc]) {
			if (!_is_little_endian())
				_memrev(list->index[sc], sizeof(DynLenFlag));
			if (IO_fwrite(list->index[sc], sizeof(DynLenFlag) +
				(DynLenFlag)*list->index[sc], 1, fp) < 0) {
				IO_fclose(fp);
				return OPS_FILE_ERR;
			}
			if (!_is_little_endian()) {
				_memrev(list->index[sc], sizeof(DynLenFlag));
				_memrev(&sc, sizeof(uint));
			}
			if (IO_fwrite(&sc, sizeof(uint), 1, fp) < 0) {
				IO_fclose(fp);
				return OPS_FILE_ERR;
			}
			if (!_is_little_endian())
				_memrev(&sc, sizeof(uint));
		}
	}
	IO_fclose(fp);
	if (!_is_little_endian()) {
		_switch_byte_order(list);
	}
	return OPS_SUCCESS;
}

list_t *list_import(const char *path)
{
	IO_FILE fp = IO_fopen(path, "rb");
	if (!fp)
		return NULL;
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if (!list) {
		IO_fclose(fp);
		return NULL;
	}
	memset(list, 0, sizeof(list_t));
	uint name_len;
	IO_fread(&name_len, sizeof(uint), 1, fp);
	if (!_is_little_endian())
		_memrev(&name_len, sizeof(uint));
	name_len++;
	char *name = (char*)malloc(name_len);
	if (!name) {
		free(list);
		IO_fclose(fp);
		return NULL;
	}
	memset(name, 0, name_len);
	IO_fread(name, name_len - 1, 1, fp);
	IO_fread((byte*)list + LIST_INFO_OFFSET, LIST_INFO_LEN, 1, fp);
	if (!_is_little_endian())
		_switch_byte_order(list);
	list->name = name;
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		list->index = (byte**)malloc(list->scale * sizeof(byte*));
		if (!list->index) {
			free(list);
			free(name);
			IO_fclose(fp);
			return NULL;
		}
		memset(list->index, 0, list->scale * sizeof(byte*));
		DynLenFlag len = 0;
		byte *tmp = NULL;
		uint id = 0;
		while (IO_fread(&len, sizeof(DynLenFlag), 1, fp)) {
			if (!_is_little_endian())
				_memrev(&len, sizeof(DynLenFlag));
			tmp = (byte*)malloc(len + sizeof(DynLenFlag));
			if (!tmp) {
				list->status |= LIST_IMPORT_ERROR;
				list->status |= LIST_MALLOC_ERROR;
				IO_fclose(fp);
				return list;
			}
			*(DynLenFlag*)tmp = len;
			IO_fread(tmp + sizeof(DynLenFlag), len, 1, fp);
			IO_fread(&id, sizeof(uint), 1, fp);
			if (!_is_little_endian())
				_memrev(&id, sizeof(uint));
			list->index[id] = tmp;
		}
		IO_fclose(fp);
		return list;
	} else if (TEST_FLAG(list->flag, LIST_STATIC_MODE)) {
		list->mem = (byte*)malloc(list->length);
		if (!list->mem) {
			free(list);
			free(name);
			IO_fclose(fp);
			return NULL;
		}
		IO_fread(list->mem, list->length, 1, fp);
		IO_fclose(fp);
		return list;
	} else {
		free(list);	
		free(name);
		IO_fclose(fp);	
		return NULL;
	}
}

list_t *list_import_static(const char *path)
{
	IO_FILE fp = IO_fopen(path, "rb");
	if (!fp)
		return NULL;
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if (!list) {
		IO_fclose(fp);
		return NULL;
	}
	memset(list, 0, sizeof(list_t));
	uint name_len;
	IO_fread(&name_len, sizeof(uint), 1, fp);
	if (!_is_little_endian())
		_memrev(&name_len, sizeof(uint));
	name_len++;
	char *name = (char*)malloc(name_len);
	if (!name) {
		free(list);
		IO_fclose(fp);
		return NULL;
	}
	memset(name, 0, name_len);
	IO_fread(name, name_len - 1, 1, fp);
	IO_fread((byte*)list + LIST_INFO_OFFSET, LIST_INFO_LEN, 1, fp);
	if (!_is_little_endian())
		_switch_byte_order(list);
	list->name = name;
	if (!TEST_FLAG(list->flag, LIST_STATIC_MODE)) {
		free(list);
		free(name);
		IO_fclose(fp);
		return NULL;
	}
	list->mem = (byte*)malloc(list->length);
	if (!list->mem) {
		free(list);
		free(name);
		IO_fclose(fp);
		return NULL;
	}
	IO_fread(list->mem, list->length, 1, fp);
	IO_fclose(fp);
	return list;
}

list_t *list_import_dynamic(const char *path)
{
	IO_FILE fp = IO_fopen(path, "rb");
	if (!fp)
		return NULL;
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if (!list) {
		IO_fclose(fp);
		return NULL;
	}
	memset(list, 0, sizeof(list_t));
	uint name_len;
	IO_fread(&name_len, sizeof(uint), 1, fp);
	if (!_is_little_endian())
		_memrev(&name_len, sizeof(uint));
	name_len++;
	char *name = (char*)malloc(name_len);
	if (!name) {
		free(list);
		IO_fclose(fp);
		return NULL;
	}
	memset(name, 0, name_len);
	IO_fread(name, name_len - 1, 1, fp);
	IO_fread((byte*)list + LIST_INFO_OFFSET, LIST_INFO_LEN, 1, fp);
	if (!_is_little_endian())
		_switch_byte_order(list);
	list->name = name;
	if (!TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		free(list);
		free(name);
		IO_fclose(fp);
		return NULL;
	}
	list->index = (byte**)malloc(list->scale * sizeof(byte*));
	if (!list->index) {
		free(list);
		free(name);
		IO_fclose(fp);
		return NULL;
	}
	memset(list->index, 0, list->scale * sizeof(byte*));
	DynLenFlag len = 0;
	byte *tmp = NULL;
	uint id = 0;
	while (IO_fread(&len, sizeof(DynLenFlag), 1, fp)) {
		if (!_is_little_endian())
			_memrev(&len, sizeof(DynLenFlag));
		tmp = (byte*)malloc(len + sizeof(DynLenFlag));
		if (!tmp) {
			list->status |= LIST_IMPORT_ERROR;
			list->status |= LIST_MALLOC_ERROR;
			IO_fclose(fp);
			return list;
		}
		*(DynLenFlag*)tmp = len;
		IO_fread(tmp + sizeof(DynLenFlag), len, 1, fp);
		IO_fread(&id, sizeof(uint), 1, fp);
		if (!_is_little_endian())
			_memrev(&id, sizeof(uint));
		list->index[id] = tmp;
	}
	IO_fclose(fp);
	return list;
}

#ifdef ENABLE_SSHM
	list_t *list_new_shared(uint scale, uint blen, uint key)
	{
		list_t *list = (list_t*)malloc(sizeof(list_t));
		if (!list)
			return NULL;
		memset(list, 0, sizeof(list_t));
		uint shmlen = 0;
		shmlen += LIST_NAME_LEN;
		shmlen += sizeof(list_t);
		SET_FLAG(list->flag, LIST_STATIC_MODE);
		SET_FLAG(list->flag, LIST_UNRESIZABLE);
		SET_FLAG(list->flag, LIST_SHARED_MEM);
		SET_FLAG(list->flag, LIST_SHARED_OWNER);
		list->length = scale * blen;
		shmlen += list->length;
		byte *shm = (byte*)create_shm(shmlen, key);
		if (!shm) {
			free(list);
			return NULL;
		}
		memset(shm, 0, shmlen);
		memcpy(shm, list, sizeof(list_t));
		free(list);
		list = (list_t*)shm;
		list->name = (char*)shm + sizeof(list_t);
		list->mem = shm + sizeof(list_t) + LIST_NAME_LEN;
		list->key = key;
		list->blen = blen;
		list->scale = scale;
		list->counter = scale;
		return list;
	}

	list_t *list_link_shared(uint len, uint key)
	{
		list_t *list = (list_t*)malloc(sizeof(list_t));
		if (!list)
			return NULL;
		byte *shm = (byte*)create_shm(len +
				sizeof(list_t) + LIST_NAME_LEN, key);
		if (!shm) {
			free(list);
			return NULL;
		}
		memcpy(list, shm, sizeof(list_t));
		list->name = (char*)shm + sizeof(list_t);
		list->mem = shm + sizeof(list_t) + LIST_NAME_LEN;
		SET_FLAG(list->flag, LIST_UNRESIZABLE);
		SET_FLAG(list->flag, LIST_SHARED_MEM);
		SET_FLAG(list->flag, LIST_SHARED_USER);
		return list;
	}

	__status list_del_shared(list_t *list)
	{
		if (TEST_FLAG(list->flag, LIST_SHARED_USER))
			return OPS_ARG_ILL;
		uint shmlen = (sizeof(list_t) +
				LIST_NAME_LEN + list->length);
		if (free_shm(shmlen, list->key))
			return OPS_SHM_ERR;
		return OPS_SUCCESS;
	}

	list_t *list_import_shared(char *path, uint key)
	{
		IO_FILE fp = IO_fopen(path, "rb");
		if (!fp)
			return NULL;
		list_t *list = (list_t*)malloc(sizeof(list_t));
		if (!list)
			return NULL;
		uint name_len;
		IO_fread(&name_len, sizeof(uint), 1, fp);
		if (!_is_little_endian())
			_memrev(&name_len, sizeof(uint));
		name_len++;
		char *name = (char*)malloc(name_len);
		if (!name) {
			free(list);
			return NULL;
		}
		memset(name, 0, name_len);
		IO_fread(name, name_len - 1, 1, fp);
		IO_fread(list, (sizeof(list_t)), 1, fp);
		if (!_is_little_endian())
			_switch_byte_order(list);
		uint shmlen = sizeof(list_t) + LIST_NAME_LEN;
		shmlen += list->length;
		byte *shm = (byte*)create_shm(shmlen, key);
		if (!shm) {
			free(list);
			free(name);
			IO_fclose(fp);
			return NULL;
		}
		IO_fread(shm + sizeof(list_t) + LIST_NAME_LEN,
			       			list->length, 1, fp);
		memcpy(shm, list, sizeof(list_t));
		strcpy((char*)shm + sizeof(list_t), name);
		free(name);
		list->name = (char*)shm + sizeof(list_t);
		list->mem = shm + sizeof(list_t) +LIST_NAME_LEN;
		IO_fclose(fp);
		return list;
	}
#endif /* ENABLE_SSHM */

void list_set_hash_remains(list_t *list, uint id)
{
	uint len = list_get_record_len(list, id);
	byte *rec = list_get_record(list, id);
	if (rec)
		memset(rec, LIST_HASH_REMAINS, len);
}

bool list_is_hash_remains(list_t *list, uint id)
{
	uint len = list_get_record_len(list, id);
	byte *rec = list_get_record(list, id);
	if (!rec)
		return false;
	while(len--) {
		if (*rec++ != LIST_HASH_REMAINS)
			return false;
	}
	return true;
}

__status list_calc_hash_id(list_t *list,
			byte *data, uint len, uint arg, uint *id, __hashFx)
{
	if (hfx == NULL)
		hfx = _djb_hash;
	uint nhash = hfx(data, len, arg) % list->scale;
	uint cid = nhash;
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		while (list->index[cid]) {
			if (list_is_hash_remains(list, cid))
				break;
			cid++;
			if (cid == list->scale)
				cid = 0;
			if (cid == nhash)
				return OPS_ERR_LISTFULL;
		}
	} else if (TEST_FLAG(list->flag, LIST_STATIC_MODE)) {
		while (*list_get_static_record(list, cid)) {
			if (list_is_hash_remains(list, cid))
				break;
			cid++;
			if (cid == list->scale)
				cid = 0;
			if (cid == nhash)
				return OPS_ERR_LISTFULL;
		}
	}
	*id = cid;
	return OPS_SUCCESS;
}

__status list_search_record_hash_mod(list_t *list,
		byte *data, uint len, uint arg, __hashFx,
			__matchFx, byte *pattern, uint plen, uint *fid)
{
	if (hfx == NULL)
		hfx = _djb_hash;
	uint nhash = hfx(data, len, arg) % list->scale;
	uint id = nhash;
	if (TEST_FLAG(list->flag, LIST_DYNAMIC_MODE)) {
		while (list->index[id]) {
			if (mfx(list->index[id] +
				sizeof(DynLenFlag), pattern, plen)) {
				*fid = id;
				return OPS_SUCCESS;
			}
			id++;
			if (id == list->scale)
				id = 0;
			if (id == nhash)
				return OPS_OBJ_NOFOUND;
		}
	} else if (TEST_FLAG(list->flag, LIST_STATIC_MODE)) {
		while (*list_get_static_record(list, id)) {
			if (mfx(list->index[id], pattern, plen)) {
				*fid = id;
				return OPS_SUCCESS;
			}
			id++;
			if (id == list->scale)
				id = 0;
			if (id == nhash)
				return OPS_OBJ_NOFOUND;
		}
	}
	return OPS_OBJ_NOFOUND;
}

void list_print_info(list_t *list, FILE *stream)
{
	FILE *fp = stream ? stream : stdout;
	if (!list) {
		fprintf(fp, "[ERROR] : list_t ERROR, stopped!\n");
		return;
	}
	if (!list->status)
		fprintf(fp, "[Status] : List works well!\n");
	else{
		fprintf(fp, "[Status] : Something wrong, stopped!\n");
		return;
	}
	if (list->name)
		fprintf(fp, "[%s]\n", list->name);
	else
		fprintf(fp, "Not named list.\n");
	if (TEST_FLAG(list->flag, LIST_STATIC_MODE))
		fprintf(fp, "[MODE] = STATIC\n");
	else
		fprintf(fp, "[MODE] = DYNAMIC\n");
	fprintf(fp, "[length] = %lld\n", list->length);
	fprintf(fp, "[scale] = %d\n", list->scale);
	fprintf(fp, "[record] = %d\n", list->counter);
	fprintf(fp, "[block length] = %d\n", list->blen);
}

void operation_status(__status ops_stat)
{
	FILE *fp = stdout;
	switch (ops_stat) {
		case OPS_SUCCESS :
			fprintf(fp, "Operation Success!\n");
			break;
		case OPS_BAD_ID :
			fprintf(fp, "Failed : Bad ID\n");
			break;
		case OPS_MALLOC_ERR :
			fprintf(fp, "Failed : Mem Alloc Failed\n");
			break;
		case OPS_DYN_ID_EXIST :
			fprintf(fp, "Failed : Record Exist\n");
			break;
		case OPS_BAD_OBJ :
			fprintf(fp, "Failed : Bad Object\n");
			break;
		case OPS_ARG_ILL :
			fprintf(fp, "Failed : Arg Illegal\n");
			break;
		case OPS_LNAME_ERR :
			fprintf(fp, "Failed : Bad LIST name\n");
			break;
		case OPS_FILE_ERR :
			fprintf(fp, "Failed : File Access Err\n");
			break;
		default :
			fprintf(fp, "Failed : Unknow Error\n");
			break;
	}
}