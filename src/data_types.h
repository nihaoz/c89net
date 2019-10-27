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

#define DATATYPE_UINT8    0x00
#define DATATYPE_UINT16   0x01
#define DATATYPE_UINT32   0x02
#define DATATYPE_UINT64   0x03
#define DATATYPE_INT8     0x10
#define DATATYPE_INT16    0x11
#define DATATYPE_INT32    0x12
#define DATATYPE_INT64    0x13
/* #define DATATYPE_FLOAT16  0x20 */
#define DATATYPE_FLOAT32  0x21
#define DATATYPE_FLOAT64  0x22

#define datatype_check(dt1, dt2) !(dt1 - dt2)

int sizeof_datatype(int dt);

const char *datatype_to_string(int dt);

#ifdef __cplusplus
	}
#endif

#endif /* _DATA_TYPES_H_ */