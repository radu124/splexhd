/*******************************************************************
(C) 2011 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/
#include "timing.h"

ttiming timing;

int ttiming::isoct(int i)
{
	return i==0 || i==o1 || i==o2 || i==o3 || i==o4 || i==o5 || i==o6 || i==o7;
}

void ttiming::init(int scale)
{
	o1=scale*1/8;
	o2=scale*2/8;
	o3=scale*3/8;
	o4=scale*4/8;
	o5=scale*5/8;
	o6=scale*6/8;
	o7=scale*7/8;
	m1=o8=scale;
	m2=scale*2;
	m3=scale*3;
	m4=scale*4;
	m5=scale*5;
	m6=scale*6;
	bugact       =m6;
	xpropag      =m1+o4;
	xplode       =m3+o6;
	exitlev      =m2;
	expdanger    =m2;
	reddiskblow  =m5;
	enemymove      = o8;
	enemymoveshort = o7;
	murphvanishing = m4;
	explosion      = m5;
}



