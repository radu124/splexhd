#ifndef __HDR_utils_h
#define __HDR_utils_h


#include "includes.h"

// defined in configuration
int tsimatch(char *&s, const char *v);

template <typename T>
T* dorealloc(T* &adr,int len)
{
	T* res=(T*) realloc(adr,len*sizeof(T));
	if (!res) exit(1);
	adr=res;
}

#endif
