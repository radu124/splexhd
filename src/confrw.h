#ifndef __HDR_confrw_h
#define __HDR_confrw_h


#include "includes.h"
#include "globals.h"

#define MULTIRDR(a,b)\
void confread_##a(char *&s, a &v) \
{ \
	 v=(a)multiplechoice(s,b); \
	 DBG(CONFIG," value " #a " %d (from:%s)\n" &(int)v &s); \
}

#define CONFWRITE_GLfloat(a) fprintf(fou,"%f",a)
#define CONFWRITE_int(a) fprintf(fou,"%d",a)
#define CONFWRITE_string(a) fprintf(fou,"%s",a.c_str())

#define DECLCONFR(a) void confread_##a(char *&s, a &v)

DECLCONFR(GLfloat);
DECLCONFR(int);
DECLCONFR(string);
DECLCONFR(mcolor);

int multiplechoice(const char *s, const char *choices);


#endif
