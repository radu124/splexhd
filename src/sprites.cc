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
#include "sprites.h"

#include "includes.h"
#include "configuration.h"
#include "readpng.h"

vector<vector<GLuint> > tiles;

int gettex(int i, int j)
{
	if (i>=tiles.size()) return 0;
	if (j>=tiles[i].size()) return 0;
	return tiles[i][j];
}

int getelectronid(int time)
{
	if (!tiles[TT_electron].size()) return 0;
	time/=10;
	return time%tiles[TT_electron].size();
}

int spritecount(int i)
{
	if (i<0 || i>=tiles.size()) return 0;
	return tiles[i].size();
}

#define _DEF_LOADIMG(a) loadImageSet((datadir+"/graphics/"#a".png").c_str(),tiles[TT_##a],0);

void init_sprites()
{
	tiles.resize(TT_last);
	_DEF_LOADIMG(tiles);
	_DEF_LOADIMG(knikcut);
	_DEF_LOADIMG(electron);
	_DEF_LOADIMG(murphwalk);
	_DEF_LOADIMG(murphfaces);
	_DEF_LOADIMG(murphvanishing);
	_DEF_LOADIMG(murphyawn);
	_DEF_LOADIMG(tronroll);
	_DEF_LOADIMG(zonkroll);
	_DEF_LOADIMG(explode);
	_DEF_LOADIMG(explodeinfo);
	_DEF_LOADIMG(vanishdisk);
	_DEF_LOADIMG(vanishinfo);
	_DEF_LOADIMG(vanishpcb);
	_DEF_LOADIMG(console);
	_DEF_LOADIMG(bug);

	_DEF_LOADIMG(bigclock);
	_DEF_LOADIMG(bigtron);
	_DEF_LOADIMG(bigdisk);
	loadImageSet((datadir+"/graphics/digits.png").c_str(),tiles[TT_digits],1);
}

void texReleaseAll()
{
	// TODO
	;
}

void texReloadAll()
{
	// TODO
	;
}



