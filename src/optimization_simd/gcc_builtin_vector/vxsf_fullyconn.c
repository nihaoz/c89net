#ifdef SET_GCC_BUILTIN_VEX

#include "vxsf_fullyconn.h"

typedef float v4sf __attribute__ ((vector_size (16)));

static float32 _v4sf_sum(v4sf vf)
{
	float32 *vfp = (float32*)&vf, s = 0;
	int n = 4;
	while (n--)
		s += *(vfp)++;
	return s;
}

void v4sf_fully_connected_float32(float32 *inp, float32 *oup,
			float32 *w, float32 *b, int iw, int ow)
{
	int i, j;
#ifdef ENABLE_OPENMP
#pragma omp parallel for private(i, j)
#endif
	for (i = 0; i < ow; ++i)
	{
		v4sf isum = {0.};
		for (j = 0; j < (iw - 4); j += 4)
		{
			isum += (*(v4sf*)(inp + j)) * (*(v4sf*)(w + iw * i + j));
		}
		oup[i] = _v4sf_sum(isum); 
		for (; j < iw; ++j)
		{
			oup[i] += (*(inp + j)) * (*(w + iw * i + j));
		}
		if (b)
			oup[i] += b[i];
	}
}

#endif /* SET_GCC_BUILTIN_VEX */