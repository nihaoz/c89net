#include <stdio.h>

#include "data_types.h"

#define DATATYPE_CHECK(condition)\
		((void)sizeof(char[1 - 2*!!(condition)]))
/*=========== compile-time test ===========*/
void ___data_util_datatype_check___()
{
	/* unsigned integer check */
	DATATYPE_CHECK(1 - sizeof(uint8));
	DATATYPE_CHECK(2 - sizeof(uint16));
	DATATYPE_CHECK(4 - sizeof(uint32));
	DATATYPE_CHECK(8 - sizeof(uint64));
	/* float32 should be single precision float */
	DATATYPE_CHECK(4 - sizeof(float32));
	/* float32 should be double precision float */
	DATATYPE_CHECK(8 - sizeof(float64));
}

int sizeof_datatype(int dt)
{
	switch (dt) {
		case DATATYPE_INT8:
		case DATATYPE_UINT8:
			return 1;
		case DATATYPE_INT16:
		case DATATYPE_UINT16:
			return 2;
		case DATATYPE_INT32:
		case DATATYPE_UINT32:
		case DATATYPE_FLOAT32:
			return 4;
		case DATATYPE_INT64:
		case DATATYPE_UINT64:
		case DATATYPE_FLOAT64:
			return 8;
		default:
			return -1;
	}
}

const char *datatype_to_string(int dt)
{
	/* switch (dt) {
		case DATATYPE_UINT8:
			return 
	} */
	return NULL;
}
