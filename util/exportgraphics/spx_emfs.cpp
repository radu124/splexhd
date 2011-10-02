#include "includes.h"
#include "spx_emfs.h"
#include "util.h"

int tileSize;
int         alphaWidth;
int         alphaHeight;
const int tileCountH=20;
const int tileCountV=22;
HDC         alphaDC=NULL;
HBRUSH blackBrush;

int         workWidth;
int         workHeight;
BITMAPINFO *workHDR;
HBITMAP     workBmp;
uint8_t      *workDat;

/**
 * workU32 stores a bitmap 8 times the resolution of a normal tile
 * (2x2 sub-tiles at 16x antialiasing)
 */
uint32_t     *workU32;
HDC         workDC;

/**
 * tilesBitmap stores all rendered tiles
 * it is 20 tiles wide and 22 tiles high
 */
HDC         tilesDC=NULL;
int         tilesWidth;
int         tilesHeight;
BITMAPINFO *tilesHDR;
HBITMAP     tilesBmp;
uint8_t    *tilesDat;
uint32_t   *tilesU32;
uint32_t   *tileswork;


HBITMAP     alphaBmp=NULL;
BITMAPINFO *alphaHDR=NULL;
uint8_t      *alphaDat;
uint32_t     *alphaU32;


uint8_t *charsDat;		

void accumulate(uint32_t &a0, uint32_t &a1, uint32_t &a2,uint32_t &a3,uint32_t val)
{
	a0+=(int) ((val>> 0) & 0xff);
	a1+=(int) ((val>> 8) & 0xff);
	a2+=(int) ((val>>16) & 0xff);
	a3+=(int) ((val>>24) & 0xff);	
}

uint32_t __fastcall reduceaccu(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	uint32_t result=0;
	result+=((uint32_t) ((a0>>4) & 0xff))<< 0;
	result+=((uint32_t) ((a1>>4) & 0xff))<< 8;
	result+=((uint32_t) ((a2>>4) & 0xff))<<16;
	result+=((uint32_t) ((a3>>4) & 0xff))<<24;
	return result;
}

uint32_t __fastcall reduceAlphaAccu(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	uint32_t result=0;
	result+=((uint32_t) ((a0>>2) & 0xff))<< 0;
	result+=((uint32_t) ((a1>>2) & 0xff))<< 8;
	result+=((uint32_t) ((a2>>2) & 0xff))<<16;
	result+=((uint32_t) ((a3>>2) & 0xff))<<24;
	return result;
}

uint32_t colormul(uint32_t color,float ratio)
{
	uint32_t result=0;
	int a0=(int) ((color>> 0) & 0xff);
	int a1=(int) ((color>> 8) & 0xff);
	int a2=(int) ((color>>16) & 0xff);
	int a3=(int) ((color>>24) & 0xff);	

	a0=(int) (a0*ratio);
	a1=(int) (a1*ratio);
	a2=(int) (a2*ratio);
	a3=(int) (a3*ratio);

	result+=((uint32_t) (a0 & 0xff))<< 0;
	result+=((uint32_t) (a1 & 0xff))<< 8;
	result+=((uint32_t) (a2 & 0xff))<<16;
	result+=((uint32_t) (a3 & 0xff))<<24;
	return result;
}

void qualitystretchNoCopy()
{
	int i,j,k,l;
	uint32_t accu0,val;

	uint32_t accu1,accu2,accu3;	
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
	{
		accu0=accu1=accu2=accu3=0;
		for (k=0; k<4; k++) for (l=0; l<4; l++)
		{
			val=workU32[(workHeight-1-i*4-k)*workWidth+j*4+l];
			accumulate(accu0,accu1,accu2,accu3,val);
		}
		val=reduceaccu(accu0,accu1,accu2,accu3);
		workU32[(workHeight-1-i)*workWidth+j]=val;
	}
/*
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
	{
		accu0=0;
		for (k=0; k<4; k++) for (l=0; l<4; l++)
		{
			val=workU32[(workHeight-1-i*4-k)*workWidth+j*4+l];
			val&=0xf0f0f0f0;
			val>>=4;
			accu0+=val;			
		}
		accu0&=0xf0f0f0f0;
		accu0|=accu0>>4;
		workU32[(workHeight-1-i)*workWidth+j]=accu0;
	}
*/
}

void makeBlackTransparent()
{
	int i,j,pos;	
	uint32_t val;
	for (i=0; i<tileSize*8; i++) for (j=0; j<tileSize*8; j++)
	{
		pos=(workHeight-1-i)*workWidth+j;
		val=workU32[pos] &0xffffff;
		if (val==0x00010101) workU32[pos]=val;
		else workU32[pos]=0xff000000+val;
	}	
}

void qualitystretch(int ti,int tj)
{
	qualitystretchNoCopy();
	// copy to tilesDC
	ti*=tileSize;
	tj*=tileSize;
	BitBlt(tilesDC,tj,ti,tileSize,tileSize,workDC,0,0,SRCCOPY);
}

void stretchToAlphaBmp(int ti,int tj)
{
	int i,j;
	qualitystretchNoCopy();
	// copy to tilesDC
	ti*=tileSize;
	tj*=tileSize;
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
		alphaU32[(alphaHeight-ti-i-1)*alphaWidth+j+tj]=workU32[(workHeight-1-i)*workWidth+j];	
}

void loademf(char *resname,int sz=0)
{
	HENHMETAFILE emfHnd;
	uint32_t emfSize;
	int fid;
	uint8_t *emfDat;
	RECT r;
	r.right=r.bottom=4*tileSize;
	if (sz==1) r.right=r.bottom=7*tileSize;
	if (sz==2) r.right=r.bottom=8*tileSize-4;
	r.top=r.left=0;
	

	emfDat=NULL;
	fid=loadfile(emfDat,emfSize,resname);
	if (sz==0) FillRect(workDC,&r,blackBrush);
	if (!emfSize) return;
	emfHnd=SetEnhMetaFileBits(emfSize,(unsigned char *) emfDat);
	PlayEnhMetaFile(workDC,emfHnd,&r);
	DeleteEnhMetaFile(emfHnd);
//	freefiledat(fid);				
}

/**
 * break a dest ID into x-y coordinates in a 8x8 resolution tile
 */
void quadrantdisp(int dest,int &di,int &dj)
{
	di=(dest&2)?0:tileSize*4;
	dj=(dest&1)?tileSize*4:0;
}

/**
 * copy between two quadrants in the workU32 area
 * each quadrant is a tile at 16x (4x4) antialiasing
 */
void copyquadrant(int dest,int src)
{
	int di,dj,si,sj;
	int i,j;
	uint32_t val;
	quadrantdisp(dest,di,dj);
	quadrantdisp(src ,si,sj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		val=workU32[(si+i)*workWidth+sj+j];
		workU32[(di+i)*workWidth+dj+j]=val;
	}
}

void udlrport(int dest,int src)
{
	int di,dj,si,sj,isrc,jsrc;
	int i,j;
	uint32_t val;
	quadrantdisp(dest,di,dj);
	quadrantdisp(src ,si,sj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		if ((i<j) ^ (i<tileSize*4-j))
		{
			jsrc=j;
			isrc=(int) ((i-tileSize*2)*1.1+tileSize*2);
		} else {
			jsrc=tileSize*4-i-1;
			isrc=(int) ((tileSize*2-j)*1.1+tileSize*2);
		}
		if (isrc<0 || isrc>=tileSize*4 || jsrc<0 || jsrc>=tileSize*4 )
			val=0; else val=workU32[(si+isrc)*workWidth+sj+jsrc];
		workU32[(di+i)*workWidth+dj+j]=val;

	}
}

void electronstar(int di,int dj,double size)
{
	int acts,asi,asj,adi,adj;
	int i,j;
	uint32_t val;
	if (size<=0) return;
	if (size>1) size=1;
	acts=(int) (size*1.3*tileSize);

	for (i=-acts; i<acts; i++) for (j=-acts; j<acts; j++)
	{
		asi=tileSize*2+i*tileSize*2/acts;
		asj=tileSize*2+j*tileSize*2/acts;
		adi=di+i;
		adj=dj+j;
		if (adi<0 || adj<0 || adi>=workWidth || adj>=workWidth) continue;
		if (asi<0 || asj<0 || asi>=workWidth || asj>=workWidth) continue;
		val=workU32[(workHeight-asi-1)*workWidth+asj+tileSize*4];
		if (!val) continue;
		workU32[(workHeight-adi-1)*workWidth+adj]=val;
	}
}


void clearquadrant(int dest, uint32_t value=0)
{
	int di,dj;
	int i,j;
	quadrantdisp(dest,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		workU32[(di+i)*workWidth+dj+j]=value;
	}
}

void settransparency(int dest)
{
	int di,dj;
	int i,j;
	uint32_t val;
	quadrantdisp(dest,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		val=workU32[(di+i)*workWidth+dj+j];
		if (val==0) val=0xff000000;
		workU32[(di+i)*workWidth+dj+j]=val;
	}
}

void cleartransparency(int dest)
{
	int di,dj;
	int i,j;
	uint32_t val;
	quadrantdisp(dest,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		val=workU32[(di+i)*workWidth+dj+j];
		val&=0x00ffffff;
		workU32[(di+i)*workWidth+dj+j]=val;
	}
}


uint32_t coloraverage(uint32_t color0, uint32_t color1, float q1)
{
	int a0=(int) ((color0>> 0) & 0xff);
	int a1=(int) ((color0>> 8) & 0xff);
	int a2=(int) ((color0>>16) & 0xff);
	int a3=(int) ((color0>>24) & 0xff);	
	int b0=(int) ((color1>> 0) & 0xff);
	int b1=(int) ((color1>> 8) & 0xff);
	int b2=(int) ((color1>>16) & 0xff);
	int b3=(int) ((color1>>24) & 0xff);	

	float q0=1-q1;
	uint32_t result=0;

	result+=(((uint32_t) (a0*q0+b0*q1)) & 0xff) << 0;
	result+=(((uint32_t) (a1*q0+b1*q1)) & 0xff) << 8;
	result+=(((uint32_t) (a2*q0+b2*q1)) & 0xff) <<16;
	result+=(((uint32_t) (a3*q0+b3*q1)) & 0xff) <<24;
	
	return result;
}

uint32_t colorblend(uint32_t color0, uint32_t color1, double q1)
{
	int a0=(int) ((color0>> 0) & 0xff);
	int a1=(int) ((color0>> 8) & 0xff);
	int a2=(int) ((color0>>16) & 0xff);
	int a3=(int) ((color0>>24) & 0xff);	
	int b0=(int) ((color1>> 0) & 0xff);
	int b1=(int) ((color1>> 8) & 0xff);
	int b2=(int) ((color1>>16) & 0xff);
	int b3=(int) ((color1>>24) & 0xff);	

	double q0=1-q1;
	uint32_t result=0;

	result+=(((uint32_t) (a0*q0+b0)) & 0xff) << 0;
	result+=(((uint32_t) (a1*q0+b1)) & 0xff) << 8;
	result+=(((uint32_t) (a2*q0+b2)) & 0xff) <<16;
	result+=(((uint32_t) (a3*q0+b3)) & 0xff) <<24;
	
	return result;
}

void interpolate(int dest,int src0,int src1,float ratio=0.5)
{
	int di,dj,si0,sj0,si1,sj1;
	int i,j;
	uint32_t val0,val1;
	quadrantdisp(dest,di ,dj );
	quadrantdisp(src0,si0,sj0);
	quadrantdisp(src1,si1,sj1);

	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		val0=workU32[(si0+i)*workWidth+sj0+j];
		val1=workU32[(si1+i)*workWidth+sj1+j];
		workU32[(di+i)*workWidth+dj+j]=coloraverage(val0,val1,ratio);
	}
}

void bugmix(int src,float alpha2=1)
{
	int si,sj,di,dj;
	int i,j;
	uint32_t val0,val1,a0,a1,a2,a3;
	float ratio;
	quadrantdisp(src,si,sj);
	quadrantdisp(0  ,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{		
		a0=a1=a2=a3=0;
		val0=workU32[(di+i)*workWidth+dj+j];
		val1=workU32[(si+i)*workWidth+sj+j];		
		val1=coloraverage(0,val1,alpha2);
		accumulate(a0,a1,a2,a3,val0);
		a0=max(max(a0,a1),max(a2,a3));
		a0=(int) (1.3*a0);
		if (a0>255) a0=255;
		ratio=a0/255.0f;
		workU32[(di+i)*workWidth+dj+j]=colorblend(val1,val0,ratio);
	}
}

void tronmix(int src)
{
	int si,sj,di,dj;
	int i,j;
	uint32_t val0,val1;//,a0,a1,a2,a3;
	float ratio;
	quadrantdisp(src,si,sj);
	quadrantdisp(0  ,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{		
		/*
		a0=a1=a2=a3=0;
		val0=workU32[(di+i)*workWidth+dj+j];
		val1=workU32[(si+i)*workWidth+sj+j];		
		accumulate(a0,a1,a2,a3,val1);
		a0=max(max(a0,a1),max(a2,a3));
		a0=(int) (1.3*a0);
		if (a0>255) a0=255;
		ratio=a0/255.0f;
		workU32[(di+i)*workWidth+dj+j]=colorblend(val0,val1,ratio);
		*/
		///*
		val0=workU32[(di+i)*workWidth+dj+j];
		ratio=(float) ((val0>>24)/256.0);
		val0&=0x00ffffff;
		val1=workU32[(si+i)*workWidth+sj+j];		
		workU32[(di+i)*workWidth+dj+j]=coloraverage(val0,val1,ratio);
		//*/

	}

}

int troncolor(int i)
{
	const int colorcycle[13]=
	{
		0xbbbbbb,
		0xd7d79c,
		0xf8f81a,
		0xff7a00,
		0xf00100,
		0xd11600,
		0xd35001,
		0x408b00,
		0x01f001,
		0x019e63,
		0x0303fc,
		0x7878ae,
		0xbbbbbb
	};
	double q1=(i%64)/64.0;
	i/=64;
	if (i>11) i-=12;
	return coloraverage(colorcycle[i],colorcycle[i+1],(float) q1);	
}

void troncopy(int dest,int src,int progress)
{
	int si,sj,di,dj;
	int i,j;
	uint32_t val0,primarycolor,tailq;
	int cc;
	quadrantdisp(src ,si,sj);
	quadrantdisp(dest,di,dj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{		
		val0=workU32[(si+i)*workWidth+sj+j];
		primarycolor=0;
		tailq=0;
		cc=val0&0x0000ff;
		
		if (val0!=0) if (cc>progress)
		{
			primarycolor=troncolor(cc);
		} else {
			if (cc>0 && cc>progress-64) tailq=cc-progress+64;
			cc=((val0&0x00ff00)>>8)+256;
			if (cc>256 && cc>progress)
			{
				primarycolor=troncolor(cc);
			} else {
				if (cc>256 && cc>progress-64) tailq=cc-progress+64;
				cc=((val0&0xff0000)>>16)+512;;
				if (cc>512 && cc>progress)
				{
					primarycolor=troncolor(cc);
				} else if (cc>512 && cc>progress-64) tailq=cc-progress+64;
			}
		}
		if (tailq>0) primarycolor=coloraverage(primarycolor,0xffffff,(float)(tailq/64.0));
		workU32[(di+i)*workWidth+dj+j]=primarycolor;
	}
}

void trondots(int dest,int src,int progress)
{
	int si,sj,di,dj,dsj,p1,p2,p3,ddi,ddj;
	int i,j;
	quadrantdisp(src ,si,sj);
	quadrantdisp(dest,di,dj);
	p1=progress/64;
	p2=p1-4;
	p3=p1-8;
	if (p2<0) p2=0;
	if (p3<0) p3=0;
	dsj=p1*tileSize;
	ddj=(int) (tileSize*0.75);
	ddi=(int) (tileSize*2.622);
	if (p1<4) for (i=0; i<tileSize/2; i++) for (j=0; j<tileSize/2; j++)
		workU32[(di+tileSize*4-1-i-ddi)*workWidth+dj+j+ddj]=workU32[(si+tileSize*4-1-i)*workWidth+sj+j+dsj];
	dsj=p2*tileSize;
	ddj=(int) (tileSize*2.75);
	if (p2<4) for (i=0; i<tileSize/2; i++) for (j=0; j<tileSize/2; j++)
		workU32[(di+tileSize*4-1-i-ddi)*workWidth+dj+j+ddj]=workU32[(si+tileSize*4-1-i)*workWidth+sj+j+dsj];
	dsj=p3*tileSize;
	ddj=(int) (tileSize*1.75);
	ddi=(int) (tileSize*0.85);
	if (p3<4) for (i=0; i<tileSize/2; i++) for (j=0; j<tileSize/2; j++)
		workU32[(di+tileSize*4-1-i-ddi)*workWidth+dj+j+ddj]=workU32[(si+tileSize*4-1-i)*workWidth+sj+j+dsj];
	
}


uint32_t getrotdpixel(int quadrant,float x,float y,float alpha)
{
	int qi,qj;
	int i,j;
	double cosa=cos(alpha);
	double sina=sin(alpha);
	x-=tileSize*2;
	y-=tileSize*2;
	quadrantdisp(quadrant,qi,qj);
	i=(int) (cosa*x+sina*y);
	j=(int) (-sina*x+cosa*y);
	i+=tileSize*2;
	j+=tileSize*2;
	if (i<0 || j<0 || i>=tileSize*4 || j>=tileSize*4) return 0;
	return workU32[(qi+i)*workWidth+qj+j];
}

void copyRotated(int dest,int src,float alpha)
{
	int di,dj;
	int i,j;
	int mini=tileSize*6;
	int minj=mini;
	int maxi=-tileSize*2;
	int maxj=maxi;
	float zoomi;
	float zoomj;
	uint32_t rotdval;
	quadrantdisp(dest,di,dj);
	
	for (i=-tileSize*2; i<tileSize*6; i++)
		for (j=-tileSize*2; j<tileSize*6; j++)
		{
			rotdval=getrotdpixel(src,(float) i,(float) j,alpha);
			if (rotdval)
			{
				if (i<mini) mini=i;
				if (j<minj) minj=j;
				if (i>maxi) maxi=i;
				if (j>maxj) maxj=j;
			}
		}
	zoomi=(float)(maxi-mini+0.0)/tileSize/4;
	zoomj=(float)(maxj-minj+0.0)/tileSize/4;

	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		rotdval=getrotdpixel(src,(float) (i*zoomi+mini),(float) (j*zoomj+minj),alpha);
		workU32[(di+i)*workWidth+dj+j]=rotdval;
	}

}

void normalRotate(int src,float alpha,float zoom=1)
{
	int i,j;	
	uint32_t rotdval;
	int zd;
	zd=(int) (tileSize*2*(1-zoom));
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		rotdval=getrotdpixel(src,(float) (i*zoom+zd),(float) (j*zoom+zd),alpha);
		workU32[(workHeight-i-1)*workWidth+tileSize*4-1-j]=rotdval;
	}
}

void consolescroll(int dest, int src,int dist)
{
	int i,j,di,dj,si,sj,vi;
	quadrantdisp(src,si,sj);
	quadrantdisp(dest,di,dj);
	for (i=tileSize/2; i<tileSize/2*5; i++) 
		for (j=tileSize/2; j<tileSize/2*5; j++)
		{
			vi=i+dist;
			while (vi>=tileSize/2*5) vi-=tileSize*2;
			workU32[(di+tileSize*4-1-i)*workWidth+j+dj]=workU32[(si+tileSize*4-1-vi)*workWidth+j+sj];
		}
}

void tronmask(int dest, int src,uint32_t color)
{
	int i,j,di,dj,si,sj;
	uint32_t val,val2;
	quadrantdisp(src,si,sj);
	quadrantdisp(dest,di,dj);
	for (i=0; i<tileSize*4; i++) 
		for (j=0; j<tileSize*4; j++)
		{
			val =workU32[(si+i)*workWidth+j+sj]&0x0000ff;
			val2=workU32[(di+i)*workWidth+j+dj];
			if (val>10 && val2>10) workU32[(di+i)*workWidth+j+dj]=color;
		}
}

void blacksquareonpcb(int src,int dist)
{
	int i,j,di,dj,v;	
	quadrantdisp(src,di,dj);
	if (dist>=tileSize*2) return;
	for (v=dist; v<tileSize*4-dist; v++) 
	{
		i=v;
		j=dist;
		workU32[(di+i)*workWidth+j+dj]=0;
		i=v;
		j=tileSize*4-1-dist;
		workU32[(di+i)*workWidth+j+dj]=0;
		i=dist;
		j=v;
		workU32[(di+i)*workWidth+j+dj]=0;
		i=tileSize*4-1-dist;
		j=v;
		workU32[(di+i)*workWidth+j+dj]=0;		
	}
}


void zonkRotate(float alpha)
{
	int i,j;
	uint32_t rotdval,origval;
		
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		origval=workU32[(workHeight-1-i)*workWidth+j];
		rotdval=getrotdpixel(3,(float) i,(float) j,-alpha);
		origval=colormul(origval,(float) (1-(rotdval&0xff)*0.0015));
		rotdval=getrotdpixel(2,(float) i,(float) j,-alpha);
		if (rotdval) origval=rotdval;
		workU32[(workHeight-1-i)*workWidth+j]=origval;
	}
}

int troncolorindex(double x,double y)
{
	// 1-80
	// 81-170
	// 171-250
	int result;
	double r;
/*
	if (x>0.48 && x<0.52) return 255;
	if (y>0.48 && y<0.52) return 155;
	return (int)(x*70);
*/
	if (x<0.25) {
		r=atan2(0.25-x,y-0.718);
		if (r< 1.05) r=1.05;
		if (r> 3.14) r=3.14;
		r-=1.04;
		result=(int) (r*38.05+1);
	} else if (x<0.75) {
		result=(int) (81+180*(x-0.25));
	} else {
		r=atan2(x-0.75,y-0.718);
		if (r< 1.05) r=1.05;
		if (r> 3.14) r=3.14;
		r-=1.04;
		result=(int) (255-r*40.9);
	}
	return result;
}

void fixtroncolorindex(int quadrant)
{
	int si,sj,i,j,ci,coord;
	double x,y;
	uint32_t val;
	quadrantdisp(quadrant,si,sj);
	for (i=0; i<tileSize*4; i++) for (j=0; j<tileSize*4; j++)
	{
		coord=(si+tileSize*4-1-i)*workWidth+sj+j;
		val=workU32[coord];
		x=((double)j)/tileSize/4;
		y=((double)i)/tileSize/4;
		if (val&0x800000) 					
		{
			val&=0x00ffff;
			ci=troncolorindex(-0.5*x+0.866*y+0.277,-0.866*x-0.5*y+1.279-0.2);			
			val|=ci<<16;
		}
		
		if (val&0x008000)
		{
			val&=0xff00ff;
			ci=troncolorindex(-0.5*x-0.866*y+1.246,0.866*x-0.5*y+0.2);
			val|=ci<<8;
		}
		if (val&0x000080) 
		{
			val&=0xffff00;
			ci=troncolorindex(x,y);
			val|=ci;
		}
		workU32[coord]=val;
	}	
}

void smlcopy(int dest,int src)
{
	int destbx,destby,srcbx,srcby;
	printf("\tduplicate tile: %d-%d to %d-%d\n",src/20,src%20,dest/20,dest%20);
	destby=(dest%20)*tileSize;
	destbx=(dest/20)*tileSize;
	srcby=(src%20)*tileSize;
	srcbx=(src/20)*tileSize;
	BitBlt(tilesDC,destby,destbx,tileSize,tileSize,tilesDC,srcby,srcbx,SRCCOPY);
}

void smlfliphoriz(int dest,int src)
{
	int destbx,destby,srcbx,srcby,i,j;
	uint32_t val;
	destby=(dest%20)*tileSize;
	destbx=(dest/20)*tileSize;
	srcby=(src%20)*tileSize;
	srcbx=(src/20)*tileSize;
	BitBlt(workDC,0,0,tileSize,tileSize,tilesDC,srcby,srcbx,SRCCOPY);
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
	{
		val=workU32[(workHeight-1-i)*workWidth+j];
		workU32[(workHeight-i-1)*workWidth+tileSize*2-j-1]=val;
	}
	BitBlt(tilesDC,destby,destbx,tileSize,tileSize,workDC,tileSize,0,SRCCOPY);
}

void smlflipfd(int dest,int src)
{
	int destbx,destby,srcbx,srcby,i,j;
	uint32_t val;		
	
	destby=(dest%20)*tileSize;
	destbx=(dest/20)*tileSize;
	srcby=(src%20)*tileSize;
	srcbx=(src/20)*tileSize;
	BitBlt(workDC,0,0,tileSize,tileSize,tilesDC,srcby,srcbx,SRCCOPY);
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
	{
		val=workU32[(workHeight-1-j)*workWidth+i];
		workU32[(workHeight-i-1)*workWidth+tileSize+j]=val;
	}
	BitBlt(tilesDC,destby,destbx,tileSize,tileSize,workDC,tileSize,0,SRCCOPY);

}

void smlrotate(int dest,int src)
{
	int destbx,destby,srcbx,srcby,i,j;
	uint32_t val;		
	
	destby=(dest%20)*tileSize;
	destbx=(dest/20)*tileSize;
	srcby=(src%20)*tileSize;
	srcbx=(src/20)*tileSize;
	BitBlt(workDC,0,0,tileSize,tileSize,tilesDC,srcby,srcbx,SRCCOPY);
	for (i=0; i<tileSize; i++) for (j=0; j<tileSize; j++)
	{
		val=workU32[(workHeight-1-j)*workWidth+i];
		workU32[(workHeight-i-1)*workWidth+tileSize*2-1-j]=val;
	}
	BitBlt(tilesDC,destby,destbx,tileSize,tileSize,workDC,tileSize,0,SRCCOPY);

}


void dotcopy(int i,int j,int typ)
{
	int x,y,v;
	int vspace=(int) (tileSize*0.4);
	int hspace=(int) (tileSize*0.35);
	typ=typ?tileSize*2:tileSize;
	for (x=0; x<vspace; x++)
		for (y=0; y<hspace; y++)
		{
			v=workU32[(workHeight-1-x-typ)*workWidth+y+tileSize*4];
			workU32[(workHeight-1-i-x)*workWidth+j+y]=v;
		}
}

void renderonechar(char * dat,int jdisp)
{
	int i,j,v;
	int vspace=(int) (tileSize*0.4);
	int hspace=(int) (tileSize*0.35);
	int vtop=(int) (tileSize*0.7);
	for (i=0; i<7; i++) 
	{
		v=dat[i];
		for (j=0; j<5; j++)
		{
			dotcopy(vtop+vspace*i,hspace*j+jdisp,(v&128)!=0);
			v<<=1;
		}
	}
}

void renderchars(int i)
{
	copyquadrant(0,1);
	int ddi,ddj,j;
	char thischar[8];
	ddi=20+i/20;
	ddj=i%20;
	
	for (j=0; j<8; j++) thischar[j]=charsDat[j*64+i*2];
	renderonechar(thischar,0);
	for (j=0; j<8; j++) thischar[j]=charsDat[j*64+i*2+1];
	renderonechar(thischar,tileSize*2);

	qualitystretch(ddi,ddj);
}

void doBigEmf(char * name, int pos,int size=1)
{
	int i;
	for (i=0; i<4; i++)
		clearquadrant(i,0x00010101);
	loademf(name,size);

	makeBlackTransparent();
	stretchToAlphaBmp(0,pos*2);	
	copyquadrant(0,1);
	stretchToAlphaBmp(0,pos*2+1);	
	copyquadrant(0,2);
	stretchToAlphaBmp(1,pos*2);	
	copyquadrant(0,3);
	stretchToAlphaBmp(1,pos*2+1);	

}

void writeTiles();

void loadsmall(char * name, int number, int di,int dj)
{
	char resname[100];
	sprintf(resname,name,number);
	loademf(resname);
	qualitystretch(di,dj);			
}

void initEmfs(int vtilesize)
{
	tileSize=vtilesize;
	int ti,tj,i;
	int tssq;
	char resname[100];
	uint32_t fid,fsize;
	charsDat=NULL;

	blackBrush=CreateSolidBrush(0);

	//bigwork=new uint32_t[tileSize*20*22];

	workWidth  = tileSize*8;
	workHeight = tileSize*8;
	workHDR    = createBmpInfo(workWidth,workHeight,32);
	workBmp    = CreateDIBSection(0,workHDR,DIB_RGB_COLORS,(void **)&workDat,NULL,0);
	workU32    = (uint32_t*) workDat;
	workDC     = getDCofBitmap(workBmp);

	tilesWidth  = tileSize*20;
	tilesHeight = tileSize*22;
	tilesHDR    = createBmpInfo(tilesWidth,tilesHeight,32);
	tilesBmp    = CreateDIBSection(0,tilesHDR,DIB_RGB_COLORS,(void **)&tilesDat,NULL,0);
	tilesU32    = (uint32_t*) tilesDat;
	tilesDC     = getDCofBitmap(tilesBmp);

	tssq       = tileSize*tileSize;
	consoleOut("Building graphics.");
	// alpha objects creation
	if (alphaDC) DeleteDC(alphaDC);
	if (alphaBmp) DeleteObject(alphaBmp);
	if (alphaHDR) free(alphaHDR);	
	alphaWidth  = tileSize*10;
	alphaHeight = tileSize*2;
	alphaHDR  = createBmpInfo(alphaWidth,alphaHeight,32);
	alphaBmp  = CreateDIBSection(0,alphaHDR,DIB_RGB_COLORS,(void **)&alphaDat,NULL,0);
	alphaU32  = (uint32_t*) alphaDat;
	alphaDC   = getDCofBitmap(alphaBmp);
	// fixed tiles
	for (ti=0; ti<40; ti++) 
	{
		if (ti!=23)
		{
			sprintf(resname,"graphics\\fixed-%02d.emf",ti);
			loademf(resname);
		} else {
			udlrport(0,1);
		}
		copyquadrant(1,0);
		if (ti==20) // shift-take red disk
		{
			for (tj=0; tj<8; tj++)
			{
				normalRotate(1,(float) 3.1415,(float) 10/(10-tj));
				qualitystretch(15,tj);
			}
			copyquadrant(0,1);
		}

		if (ti==19) // console scroll
		{
			copyquadrant(2,0);
			for (tj=0; tj<8; tj++)
			{
				consolescroll(1,2,tj*tileSize/4);
				copyquadrant(0,1);
				qualitystretch(16,tj+8);
			}
			copyquadrant(0,2);			
		}

		if (ti==2) // shift-take pcb
		{
			copyquadrant(2,0);
			int order[8]={0,2,4,6,1,3,5,7};
			int v,_start,_stop;
			for (tj=0; tj<8; tj++)
			{
				_start=order[tj];
				_stop=(_start+1)*tileSize/4+1;
				_start*=tileSize/4;
				for (v=_start; v<_stop; v++)
					blacksquareonpcb(1,v);
				copyquadrant(0,1);
				qualitystretch(16,tj);
			}
			copyquadrant(0,2);			
		}


		qualitystretch(ti/20,ti%20);
	}
	smlfliphoriz(11,9);
	smlflipfd(10,9);
	smlflipfd(12,11);
	smlflipfd(38,26);
	smlflipfd(39,27);
	smlflipfd(21,22);
	smlcopy(25,2);
	for (i=9; i<13; i++) smlcopy(i+4,i);
	consoleOut("Murph having lunch.");
	// murphy eating
	for (tj=0; tj<20; tj++)
	{
		sprintf(resname,"graphics\\murph-eating-%02d.emf",tj);
		loademf(resname);
		qualitystretch(4,tj);
		smlfliphoriz(100+tj,80+tj);
	}
	smlcopy(3,80);	
	
	// bug
	loademf("graphics\\fixed-02.emf");		
	copyquadrant(1,0);
	for (tj=0; tj<20; tj++)
	{
		//copyquadrant(0,1);
		sprintf(resname,"graphics\\bug-%02d.emf",tj);
		loademf(resname);
		bugmix(1);
		qualitystretch(18,tj);
	}

	// text display
	loademf("graphics\\display-0.emf");
	qualitystretch(19,12);
	loademf("graphics\\display-1.emf");
	copyquadrant(1,0);
	if (fid=loadfile(charsDat,fsize,"graphics\\chars6.dat")==512)
	{
		for (i=0; i<32; i++) renderchars(i);		
	}
	freefiledat(fid);
	
	// scissors
	for (tj=0; tj<10; tj++)
	{
		sprintf(resname,"graphics\\scissors-%1d.emf",tj);
		loademf(resname);
		if (tj==0) copyquadrant(1,0);
		qualitystretch(8,tj);
		smlrotate(170+tj,160+tj);
		smlrotate(180+tj,170+tj);
		smlrotate(190+tj,180+tj);
		smlcopy(17,160);
	}
	for (tj=0; tj<20; tj++)
	{
		normalRotate(1,(float) (tj*0.157));
		qualitystretch(11,tj);
		normalRotate(1,(float) (tj*0.157+3.14));		
		qualitystretch(10,tj);
	}

	consoleOut("Rolling infotron.");
	// infotron rotating
	loademf("graphics\\infotron-base.emf");
	fixtroncolorindex(0);
	copyquadrant(1,0);
	loademf("graphics\\infotron-dots.emf");
	copyquadrant(2,0);
	
	for (tj=0; tj<12; tj++)
	{
		troncopy(0,1,64*tj+63);
		trondots(0,2,64*tj+63);
		qualitystretch(15,tj+8);
	}
	troncopy(0,1,0);
	trondots(0,2,0);
		
	copyquadrant(1,0);	
	copyquadrant(3,0);
	for (tj=0; tj<20; tj++)
	{
		copyRotated(0,1,(float)(tj*0.314));
		qualitystretch(6,tj);
	}
	smlcopy(4,120);


	copyquadrant(0,3);
	
	clearquadrant(2);
	settransparency(2);
	consoleOut("Infotrons creeping in.");
	// infotron formation
	for (tj=0; tj<10; tj++)
	{
		sprintf(resname,"graphics\\infexplode-%1d.emf",tj);
		loademf(resname);
		//settransparency(0);
		copyquadrant(1,0);
		if (tj>=5) bugmix(3,(tj-5)/6.0f);
		qualitystretch(13,tj*2+1);
		interpolate(0,1,2);		
		if (tj>=5) bugmix(3,(tj-5)/6.0f);
		qualitystretch(13,tj*2);
		copyquadrant(2,1);
	}

	consoleOut("Rolling stones.");
	// zonk rolling
	loademf("graphics\\zonk-0.emf");
	copyquadrant(1,0);
	loademf("graphics\\zonk-1.emf");
	copyquadrant(2,0);
	loademf("graphics\\zonk-2.emf");
	copyquadrant(3,0);
	for (tj=0; tj<20; tj++)
	{
		copyquadrant(0,1);
		zonkRotate((float)(tj*0.314));
		qualitystretch(7,tj);
	}
	smlcopy(1,140);


	consoleOut("Going up in smoke.");
	// explosion
	clearquadrant(2);
	for (tj=0; tj<10; tj++)
	{
		sprintf(resname,"graphics\\explosion-%1d.emf",tj);
		loademf(resname);
		copyquadrant(1,0);
		qualitystretch(12,tj*2+1);
		interpolate(0,1,2);		
		qualitystretch(12,tj*2);
		copyquadrant(2,1);
	}
	
	consoleOut("Starring electrons.");
	// electron
	sprintf(resname,"graphics\\electron-star.emf",tj);
	loademf(resname);
	copyquadrant(1,0);
	for (ti=0; ti<20; ti++) 
	{
		clearquadrant(0);
		electronstar((int)(tileSize*1.1),(int)(tileSize*1  ),0.7+0.3*sin(ti*0.314+0.0));
		electronstar((int)(tileSize*2.1),(int)(tileSize*2  ),0.7+0.3*sin(ti*0.314+2.0));
		electronstar((int)(tileSize*1.1),(int)(tileSize*2.9),0.7+0.3*sin(ti*0.314+1.0));
		electronstar((int)(tileSize*3.0),(int)(tileSize*1.1),0.7+0.3*sin(ti*0.314+4.0));
		electronstar((int)(tileSize*2.9),(int)(tileSize*2.9),0.7+0.3*sin(ti*0.314+3.0));
		qualitystretch(14,ti);
	}
	smlcopy(24,280);
	
	//loademf("graphics\\murph-push.emf");
	//qualitystretch(2,0);
	//smlfliphoriz(41,40);
	// special murph stances
	for (tj=0; tj<5; tj++)
	{
		loadsmall("graphics\\special-%02d.emf",tj,2,tj*2);
		smlfliphoriz(41+tj*2,40+tj*2);
	}
	consoleOut("Murph taking a nap.");
	for (tj=0; tj<20; tj++)
		loadsmall("graphics\\bored-%02d.emf",tj,3,tj);
	for (tj=0; tj<10; tj++)
		loadsmall("graphics\\leave-%02d.emf",tj,2,tj+10);
	
	// alpha zone
	doBigEmf("graphics\\big-00.emf",0);
	doBigEmf("graphics\\big-01.emf",1);
	doBigEmf("graphics\\big-02.emf",2);
	doBigEmf("graphics\\big-03.emf",3,2);
	doBigEmf("graphics\\big-04.emf",4,2);

	// murph stances
	/*
	for (ti=0; ti<40; ti++) 
	{
		sprintf(resname,"graphics\\murphs-%02d.emf",ti);
		loademf(resname);
		qualitystretch(ti/20+19,ti%20);
	}
	*/

	DeleteDC(workDC);
	DeleteObject(workBmp);
	free(workHDR);	

	writeTiles();
}

void writeTiles()
{
	BITMAPFILEHEADER bfh;
	int fou;
	int datasize=20*22*4*tileSize*tileSize;
	memset(&bfh,0,sizeof(bfh));
	bfh.bfType=0x4D42;
	bfh.bfSize=datasize+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits=sizeof(bfh)+sizeof(BITMAPINFOHEADER);
	fou=_open("output.bmp",_O_CREAT | _O_WRONLY | _O_BINARY);
	write(fou,&bfh,sizeof(bfh));
	write(fou,tilesHDR,sizeof(BITMAPINFOHEADER));
	write(fou,tilesU32,datasize);
	_close(fou);

	datasize=5*4*4*tileSize*tileSize;
	memset(&bfh,0,sizeof(bfh));
	bfh.bfType=0x4D42;
	bfh.bfSize=datasize+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits=sizeof(bfh)+sizeof(BITMAPINFOHEADER);
	fou=_open("alpha.bmp",_O_CREAT | _O_WRONLY | _O_BINARY);
	write(fou,&bfh,sizeof(bfh));
	write(fou,alphaHDR,sizeof(BITMAPINFOHEADER));
	write(fou,alphaU32,datasize);
	_close(fou);

}
