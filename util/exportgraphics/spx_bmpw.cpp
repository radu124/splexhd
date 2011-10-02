#include "includes.h"

BITMAPINFO* createBmpInfo(int width,int height,int bpp)
{
	BITMAPINFO * a;
	BITMAPINFOHEADER *bi;
	int alocsize=sizeof(BITMAPINFOHEADER);
	switch (bpp)
	{
	case 1: 
		alocsize+=sizeof(RGBQUAD)*2;
		break;
	case 2: 
		alocsize+=sizeof(RGBQUAD)*4;
		break;
	case 4:
		alocsize+=sizeof(RGBQUAD)*16;
		break;
	case 16:
	case 24:
	case 32:
		break;
	default:
		alocsize+=sizeof(RGBQUAD)*256;
		break;
	}	
	
	a=(BITMAPINFO *) malloc(alocsize);
	
	bi=&(a->bmiHeader);
	bi->biSize=40;
	bi->biBitCount=bpp;
	bi->biClrUsed=0;
	bi->biClrImportant=0;
	bi->biWidth=width;
	bi->biHeight=height;
	bi->biPlanes=1;
	bi->biCompression=0;
	bi->biSizeImage=width*height;
	bi->biXPelsPerMeter=10000;
	bi->biYPelsPerMeter=10000;
	return a;
}


HDC getDCofBitmap(HBITMAP hbm)
{
	HDC dc;
/*
	DEVMODE dm;
	dm.dmDeviceName[0]=0;
	dm.dmSpecVersion=DM_SPECVERSION;
	dm.dmDriverVersion=0;
	dm.dmSize=sizeof(dm);
	dm.dmFields=DM_BITSPERPEL;
	dm.dmBitsPerPel=32;
	dc=CreateDC("DISPLAY",NULL,NULL,&dm);
*/
	dc=CreateCompatibleDC(NULL);
	SelectObject(dc,hbm);
	return dc;
}
