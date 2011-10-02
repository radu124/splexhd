#ifndef _HDR_UTIL_
#define _HDR_UTIL_

#include "includes.h"
#define consoleOut(a) printf("%s\n",a)

int loadfile(uint8_t *& dat, uint32_t &size,const char *name);
void freefiledat(int fid);

BITMAPINFO* createBmpInfo(int width,int height,int bpp);
HDC getDCofBitmap(HBITMAP hbm);

#endif

