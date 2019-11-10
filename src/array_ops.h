#ifndef _ARRAY_OPS_H_
#define _ARRAY_OPS_H_

#ifdef __cplusplus
	extern "C" {
#endif

void add_to_array(void *arr, int arrlen, void *x, int dt);

void mul_to_array(void *arr, int arrlen, void *x, int dt);

#ifdef __cplusplus
	}
#endif

#endif /* _ARRAY_OPS_H_ */