#include "includes.h"

int loadfile(uint8_t *& dat, uint32_t &size,const char *name)
{
	int f;
	char fn[1024];
	size=0;
	sprintf(fn,"splex.pak\\%s",name);
	f=open(fn,O_RDONLY);
	if (f==-1) return 0;
	size=lseek(f,0,SEEK_END);
	if (size<=0) 
	{
		size=0;
		close(f);
		return 0;
	}
	lseek(f,0,SEEK_SET);
	dat=(uint8_t *)malloc(size);
	read(f,dat,size);
	close(f);
	return 1;
}

// ignore, we can afford a memory leak here
void freefiledat(int fid)
{
	;
}
