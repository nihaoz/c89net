#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef uint8
	#warning data_types.h: uint8 already defined
	#undef uint8
	#define uint8 unsigned char
#else
	#define uint8 unsigned char
#endif

#ifdef uint16
	#warning data_types.h: uint16 already defined
	#undef uint16
	#define uint16 unsigned short
#else
	#define uint16 unsigned short
#endif

#ifdef uint32
	#warning data_types.h: uint32 already defined
	#undef uint32
	#define uint32 unsigned int
#else
	#define uint32 unsigned int
#endif

#ifdef uint64
	#warning data_types.h: uint64 already defined
	#undef uint64
	#define uint64 unsigned long long
#else
	#define uint64 unsigned long long
#endif

#ifdef int8
	#warning data_types.h: int8 already defined
	#undef int8
	#define int8 char
#else
	#define int8 char
#endif

#ifdef int16
	#warning data_types.h: int16 already defined
	#undef int16
	#define int16 short
#else
	#define int16 short
#endif

#ifdef int32
	#warning data_types.h: int32 already defined
	#undef int32
	#define int32 int
#else
	#define int32 int
#endif

#ifdef int64
	#warning data_types.h: int64 already defined
	#undef int64
	#define int64 long long
#else
	#define int64 long long
#endif

#ifdef float32
	#warning data_types.h: float32 already defined
	#undef float32
	#define float32 float
#else
	#define float32 float
#endif

#ifdef float64
	#warning data_types.h: float64 already defined
	#undef float64
	#define float64 double
#else
	#define float64 double
#endif

/*
 *
 * 7 6 5 4 3  ~  0
 * | | | | |_____|
 * | | | |    |____sub-class code
 * | | |
 * | |
 * | |______integer or float(0/1)
 * |
 * |______unsigned or signed(0/1)
 *
 * Sub class code:
 * n:   type ^ n
 *
 */

#define DATATYPE_FLAG_POINT 6
#define DATATYPE_FLAG_SIGN  7

#define DATATYPE_UINT8    0x03
#define DATATYPE_UINT16   0x04
#define DATATYPE_UINT32   0x05
#define DATATYPE_UINT64   0x06
#define DATATYPE_INT8     0x83
#define DATATYPE_INT16    0x84
#define DATATYPE_INT32    0x85
#define DATATYPE_INT64    0x86
/* #define DATATYPE_FLOAT16  0xC4 */
#define DATATYPE_FLOAT32  0xC5
#define DATATYPE_FLOAT64  0xC6

#define datatype_check(dt1, dt2) (!(dt1 - dt2))

int sizeof_datatype(int dt);

int is_datatype_float(int dt);

int is_datatype_integer(int dt);

const char *datatype_to_string(int dt);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_TYPES_H_ */