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
#include "objOrange.h"

#include "gamedata.h"
#include "timing.h"
#include "sprites.h"
#include "display.h"
#include "objExplosion.h"

void doobj_orange(int loc)
{
	if (ldt[loc].counter!=0) return;
	if (ldt[below(loc)].isfree())
	{
		ldt[below(loc)].set(TX_ORANGE | TH_MOVEIN | TH_DOWN, timing.zonkmove);
		ldt[loc].set(TX_MOVEOUT | TH_DOWN, timing.zonkmove);
		return;
	}
	if (ldt[loc].movdirection() == TH_DOWN)
	{
		// hit something
		explode_region(loc);
	}
}

void display_orange(int loc)
{
	GLfloat x,y;
	computeloc(loc,y,x);
	painttex(y,x,TB_ORANGE,TT_tiles,0);
}



