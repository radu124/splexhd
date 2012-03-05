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
#include "objVanishing.h"

#include "display.h"
#include "gamedata.h"
#include "sprites.h"
#include "play.h"
#include "timing.h"

void doobj_vanishing(int loc)
{
	if (ldt[loc].counter) return;
	if (ldt[loc].movfull()==TV_EXPLODEINFO) ldt[loc].set(TX_INFOTRON);
	else ldt[loc].set(0);
}

void display_vanishing(int loc)
{
	int typ=ldt[loc].movfull();
	int tileset=TT_murphvanishing;
	int progress;
	progress=timing.zonkmove;
	switch (typ)
	{
	case TB_PCB:
		tileset=TT_vanishpcb;
		break;
	case TB_MURPHY:
		tileset=TT_murphvanishing;
		progress=timing.murphvanishing;
		break;
	case TB_INFOTRON:
		tileset=TT_vanishinfo;
		break;
	case TV_REDDISK:
		tileset=TT_vanishdisk;
		break;
	case TV_EXPLODE:
		tileset=TT_explode;
		progress=timing.explosion;
		break;
	case TV_EXPLODEINFO:
		tileset=TT_explodeinfo;
		progress=timing.explosion;
		break;
	}
	GLfloat x,y;
	computeloc(loc,y,x,1);
	progress=animprogress(ldt[loc].counter,progress,0,spritecount(tileset)-1);
	painttex(y,x,progress,tileset,0);
}




