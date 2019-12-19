/*
 * Memory manager...
 */
#ifndef _MEMMGR_H_
#define _MEMMGR_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef LESS_RESOURCE
	#define MEMMGR_INIT_REC 512
#else
	#define MEMMGR_INIT_REC 8192
#endif

#define MEMMGR_REC_NAME_BUF 256

#define MEMMGR_STATE_OK   0
#define MEMMGR_ERR_INIT   1
#define MEMMGR_ERR_NOINIT 2
#define MEMMGR_WARN_INIT  3
#define MEMMGR_ERR_ALLOC  4
/* hash fx error on calc or search */
#define MEMMGR_ERR_CHASH  5
#define MEMMGR_ERR_SHASH  6
#define MEMMGR_ERR_EXSIT  7
/* data type not support */
#define MEMMGR_ERR_DTYPE  8
/* record name not set */
#define MEMMGR_ERR_NONAME 9

#define MEMMGR_REC_TYPE_FEATURE_MAP 10
#define MEMMGR_REC_TYPE_CNN_PARA    11

/*
 * Memory manager record type abbreviations
 */
#define MM_ABBR_FEATURE_MAP "fmap"
#define MM_ABBR_CNN_PARA    "cnnp"

#include <stdio.h>

int memmgr_init(void);

int memmgr_clear(void);

int memmgr_add_record(int type, void *sp);

void *memmgr_get_record(int type, const char *name);

void memmgr_del_record(int type, const char *name);

void debug_fprint_memmgr_info(FILE *fp);

void debug_fprint_memmgr_list(FILE *fp);

#ifdef __cplusplus
	}
#endif

#endif /* _MEMMGR_H_ */