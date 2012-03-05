#ifndef __HDR_message_h
#define __HDR_message_h


#include "includes.h"

class MESSAGEWRITER
{
public:
	MESSAGEWRITER(FILE *f):fou(f),contents(""),pos(0){;}
	MESSAGEWRITER & operator ,(const string b);
	MESSAGEWRITER & operator ,(const char * b);
	MESSAGEWRITER & operator ,(unsigned int b);
	MESSAGEWRITER & operator ,(int b);
	MESSAGEWRITER & operator ,(float b);
	MESSAGEWRITER & operator ,(double b);
	~MESSAGEWRITER();
	int skiptochar(string);
	FILE *fou;
	string contents;
	int pos;
	int ipos;
	int lpos;
	string modifier;
};

#define __COND_DBG_OUT(cond,...) do { if (!(cond)) break; { MESSAGEWRITER __MW(stderr); __MW , __VA_ARGS__; }} while(0)

#define __FILE_OUT(fou,...) do { {MESSAGEWRITER __MW(fou); __MW , __VA_ARGS__;} } while(0)

#define assume(cond,...) \
	do { if (cond) break; \
	{ MESSAGEWRITER __MW(stderr); __MW , "\n!!! ERROR !!! in " ,__func__ ,"() at " , __FILE__ ,":",__LINE__ , "\n" , __VA_ARGS__ , "\n\n"; \
	} assert(0); } while(0)

#define assret(cond,...) \
	do { if (cond) break; \
	{ MESSAGEWRITER __MW(stderr); __MW , "\n!!! FAIL !!! in " ,__func__ ,"() at " , __FILE__ ,":",__LINE__ , "\n" , __VA_ARGS__ , "\n\n"; \
	} return 0; } while(0)
	
#define warnif(cond,...) \
	do { if (!cond) break; \
	{ MESSAGEWRITER __MW(stderr); __MW , "\n!!! WARNING !!! in " ,__func__ ,"() at " , __FILE__ ,":",__LINE__ , "\n" , __VA_ARGS__ , "\n\n"; \
	} } while(0)

#define assume_paranoid(cond,...) assume(cond,__VA_ARGS__)

#define MESSAGE(...) __FILE_OUT(stdout,__VA_ARGS__)



#endif
