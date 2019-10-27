#ifndef _LIST_H_
#define _LIST_H_

#ifndef __cplusplus
	#define bool  unsigned char
	#define true  1
	#define false 0
#else
	extern "C" {
#endif

#ifndef uchar
	#define uchar unsigned char
#endif
#ifndef byte
	#define byte  unsigned char
#endif
#ifndef uint
	#define uint  unsigned int
#endif
#ifndef ulong
	#define ulong unsigned long long
#endif

#ifndef INT_INFINITY
	#define INT_INFINITY ~0
#endif

typedef uchar __status;

/*========== operation status ===========*/ 
#define OPS_STATUS_
#define OPS_SUCCESS         0x00
#define OPS_BAD_ID          0x01
#define OPS_MALLOC_ERR      0x02
#define OPS_DYN_ID_EXIST    0x03
#define OPS_BAD_OBJ         0x04
#define OPS_ARG_ILL         0x05
#define OPS_LNAME_ERR       0x06
#define OPS_FILE_ERR        0x07
#define OPS_SHM_ERR         0x08
#define OPS_ERR_LISTFULL    0x09
#define OPS_OBJ_NOFOUND     0x0A

#define SET_BIT(val, bitn) (val |=(1<<(bitn)))
#define CLR_BIT(val, bitn) (val&=~(1<<(bitn)))
#define GET_BIT(val, bitn) (val & (1<<(bitn)))

#define SET_FLAG(flag, bft) \
	if(bft & 0x0F)SET_BIT(flag, bft >> 4); else CLR_BIT(flag, bft >> 4);
#define TEST_FLAG(flag, bft) \
	!(((bft & 0x0F) << (bft >> 4)) ^ (flag & (1 << (bft >> 4))))

#define DYN_LENGTH_TYPE uint
typedef DYN_LENGTH_TYPE DynLenFlag;

/* Do not edit LIST_NAME_LEN unless necessary */
#ifndef LIST_NAME_LEN
	#define LIST_NAME_LEN 128
#endif

#ifndef LIST_PATH_LEN
	#define LIST_PATH_LEN 256
#endif

/*
 * Record on dynamic mode :
 * uint length | record |, when export :
 * uint length | record | id
 * Notice: length of a dynamic record not account the DynLenFlag
 *
 * index     record(Each record was allocated memory by "malloc")
 * index0----->rec0
 * index1----->rec1
 *        ...
 * indexX----->recX
 * X = list.scale - 1
 * ==============================================================
 * Record on static mode :
 * list.mem :
 * record 0 | record 1 | ... | record X
 * _________
 *          |
 *          |---->list.blen
 */

typedef struct {
	char *name;     /* name of LIST */
	byte *mem;      /* memory pool */
	byte **index;   /* index table */
	ulong length;   /* length of memory pool */
	uint key;       /* key of shared mem */
	uint counter;   /* record counter */
	uint blen;      /* length of a record */
	uint scale;     /* scale */
	byte flag;      /* list's mode flag */
	byte status;    /* work status flag */
	byte placeholder[6];
} list_t;

struct list_info {
	ulong length;   /* length of memory pool */
	uint key;       /* key of shared mem */
	uint counter;   /* record counter */
	uint blen;      /* length of a record */
	uint scale;     /* scale */
	byte flag;      /* list's mode flag */
	byte status;    /* work status flag */
	byte placeholder[6];
};

#define LIST_INFO_LEN sizeof(struct list_info)
#define LIST_INFO_OFFSET sizeof(list_t) - LIST_INFO_LEN

/*============== mode flag ==============*/
#define LIST_FLAG_DEFAULT           0x00
#define LIST_STATIC_MODE            0x00
#define LIST_DYNAMIC_MODE           0x01
#define LIST_RESIZABLE              0x50
#define LIST_UNRESIZABLE            0x51
#define LIST_SHARED_USER            0x60
#define LIST_SHARED_OWNER           0x61
#define LIST_NOT_SHARED             0x70
#define LIST_SHARED_MEM             0x71

/*============= status flag =============*/
/* LIST_STATUS_ALRIGHT MUST BE 0x00 */
#define LIST_STATUS_ALRIGHT         0x00

#define LIST_MALLOC_ERROR           0x01
#define LIST_IMPORT_ERROR           0x02

/* Tips:
 * Overload New method under cpp
 * list_t *list_new(uint scale);
 * list_t *list_new(uint scale, uint blen);
 */
list_t *list_new_static(uint scale, uint blen);
list_t *list_new_dynamic(uint scale);

__status list_del(list_t *list);
__status list_del_static(list_t *list);
__status list_del_dynamic(list_t *list);

__status list_resize(list_t *list, uint newScale);
__status list_resize_static(list_t *list, uint newScale);
__status list_resize_dynamic(list_t *list, uint newScale);

__status list_set_name(list_t *list, const char *name);

__status list_set_record(list_t *list, uint id,
	byte *record, uint len);
__status list_set_static_record(list_t *list, uint id,
	byte *record, uint len);
__status list_set_dynamic_record(list_t *list, uint id,
	byte *record, uint len);

bool list_test_record(list_t *list, uint id, uint offset,
	uint nbyte);

byte *list_get_record(list_t *list, uint id);
byte *list_get_static_record(list_t *list, uint id);
byte *list_get_dynamic_record(list_t *list, uint id);

uint list_get_record_len(list_t *list, uint id);
/* #define list_get_static_record_len */
#define list_get_dynamic_record_len(record) \
	(DynLenFlag)*(record - sizeof(DynLenFlag))

__status list_del_record(list_t *list, uint id);
__status list_del_static_record(list_t *list, uint id);
__status list_del_dynamic_record(list_t *list, uint id);

__status list_swap_record(list_t *list,
	uint id1, uint id2);
__status list_swap_static_record(list_t *list,
	uint id1, uint id2);
__status list_swap_dynamic_record(list_t *list,
	uint id1, uint id2);

/*
 * When export a list to file, you can use zlib to compress the 
 * data if you have zlib on your system
 */

/* #define ENABLE_ZLIB */

#ifdef ENABLE_ZLIB
	#define IO_FILE     gzFile
	#define IO_fopen    gzopen
	#define IO_fclose   gzclose
	#define IO_fread(buf, size, count, fp) \
		gzread(fp, buf, size)
	#define IO_fwrite(buf, size, count, fp) \
		gzwrite(fp, buf, size)
	#define IO_fflush   gzflush
	#define IO_fseek    gzseek
	#define IO_ftell    gztell
#else
	#define IO_FILE     FILE*
	#define IO_fopen    fopen
	#define IO_fclose   fclose
	#define IO_fread    fread
	#define IO_fwrite   fwrite
	#define IO_fflush   fflush
	#define IO_fseek    fseek
	#define IO_ftell    ftell
#endif /* ENABLE_ZLIB */

#define LIST_DEF_EXPORT_FILE_MODE "wb"

__status list_export(list_t *list, const char *path,
	const char *mode);

/*
 * Exported file structure of Static LIST
 * name_len | list->name | list | list->mem  
 */
__status list_export_static(list_t *list, const char *path,
	const char *mode);

/*
 * Exported file structure of Dynamic LIST
 * name_len | list->name | list | *index[0:N]
 */
__status list_export_dynamic(list_t *list, const char *path,
	const char *mode);

list_t *list_import(const char *path);
list_t *list_import_static(const char *path);
list_t *list_import_dynamic(const char *path);

/*
 * Sharedlist is tlist's extension to create list_t at shared memory space.
 * Notice that sharedlist not support tlist which was created as Dynamic
 * mode, cuz that will create too many shm handles.
 */

/* #define ENABLE_SSHM */

/* list_t | name | mem */

#ifdef ENABLE_SSHM
	list_t *list_new_shared(uint scale, uint blen, uint key);
	list_t *list_link_shared(uint len, uint key);
	#define list_shared_shm_len(list) \
		(sizeof(LIST) + LIST_NAME_LEN + list->length)
	__status list_del_shared(list_t *list);
	#define list_export_shared list_export_static
	list_t *list_import_shared(char *path, uint key);
#endif /* ENABLE_SSHM */

/*
 * simple hash function
 */
#define __hashFx uint hfx(byte*, uint, uint)
#define __matchFx bool mfx(byte*, byte*, uint)

#define list_set_unresizable(list) \
	SET_FLAG(list->flag, LIST_UNRESIZABLE)
#define list_set_resizable(list) \
	SET_FLAG(list->flag, LIST_RESIZABLE)

#define LIST_HASH_REMAINS 0xCE

void list_set_hash_remains(list_t *list, uint id);
/* #define list_del_hash_record list_set_hash_remains */
bool list_is_hash_remains(list_t *list, uint id);

__status list_calc_hash_id(list_t *list,
			byte *data, uint len, uint arg, uint *id, __hashFx);

__status list_search_record_hash_mod(list_t *list,
		byte *data, uint len, uint arg, __hashFx,
			__matchFx, byte *pattern, uint plen, uint *fid);

/*================= Debug =================*/
#define list_ops operation_status
void list_print_info(list_t *list, FILE *stream);
void operation_status(__status ops_stat);

#endif /* _LIST_H_ */

#ifdef __cplusplus
	}
#endif
