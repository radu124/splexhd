#ifndef __HDR_sprites_h
#define __HDR_sprites_h

#include "includes.h"

void init_sprites();
int gettex(int i, int j);
int spritecount(int i);

void texReleaseAll();
void texReloadAll();

int getelectronid(int time);

extern vector<vector<GLuint> > tiles;

enum enum_tilesets {
	TT_tiles,
	TT_murphfaces,
	TT_murphwalk,
	TT_murphvanishing,
	TT_murphyawn,
	TT_tronroll,
	TT_zonkroll,
	TT_knikcut,
	TT_knikturn,
	TT_explode,
	TT_explodeinfo,
	TT_electron,
	TT_vanishdisk,
	TT_vanishinfo,
	TT_vanishpcb,
	TT_console,
	TT_bug,
	TT_bigclock,
	TT_bigtron,
	TT_bigdisk,
	TT_digits,
	TT_last};


#endif
