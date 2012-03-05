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
#include "objZonk.h"

#include "gamedata.h"
#include "timing.h"
#include "display.h"
#include "sprites.h"
#include "objExplosion.h"
#include "play.h"

int zonktryfall(int loc)
{
	int base=ldt[loc].xtyp();
	int BELOW=below(loc);
	if (ldt[BELOW].isfree())
	{
		ldt[loc].set(TX_MOVEOUT | TH_DOWN, timing.zonkfall);
		ldt[BELOW].set(base | TH_MOVEIN | TH_DOWN, timing.zonkfall);
		return 1;
	}
	return 0;
}

int freeforroll(int loc)
{
	int BELOWROLL=below(loc);
	return ldt[loc].isfree() && ldt[BELOWROLL].isfree();
}

int zonktryslip(int loc)
{
	int base=ldt[loc].xtyp();
	int BELOW=below(loc);
	int LEFTOF=leftof(loc);
	int RIGHTOF=rightof(loc);

	if (ldt[BELOW].basetyp()!=TX_INFOTRON &&
		ldt[BELOW].basetyp()!=TX_ZONK &&
		!ldt[BELOW].isic()) return 0;
	if (freeforroll(RIGHTOF))
	{
		ldt[RIGHTOF].set(base | TH_MOVEIN | TH_RIGHT,timing.zonkmove);
		ldt[loc].set(TX_MOVEOUT | TH_RIGHT,timing.zonkmove);
		return 1;
	}
	if (freeforroll(LEFTOF))
	{
		ldt[LEFTOF].set(base | TH_MOVEIN | TH_LEFT, timing.zonkmove);
		ldt[loc].set(TX_MOVEOUT | TH_LEFT,timing.zonkmove);
		return 1;
	}
	return 0;
}

int zonkhit(int loc, int base)
{
	int BELOW=below(loc);
	game.sfxzonkleft=game.sfxzonkright=1;
	if (ldt[BELOW].xtyp()==TX_ORANGE)
	{
		explode_region(BELOW);
		return 1;
	}
	if (ldt[BELOW].xtyp()==TX_YELLOW && base==TX_INFOTRON)
	{
		explode_region(BELOW);
		return 1;
	}
	if (ldt[BELOW].xtyp()==TX_MURPH)
	{
		explode_region(BELOW);
		game.ended=1;
		return 1;
	}
	return 0;
}

void doobj_zonk(int loc)
{
	int base=ldt[loc].xtyp();
	int th=ldt[loc].typ & 0x0f;
	int BELOW=below(loc);
	if (ldt[loc].counter>0) return;
	if (game.freezezonks) return;
	switch (th)
	{
	case TH_NORMAL:
		if (ldt[BELOW].isfree()) ldt[loc].set(base | TH_PREPAREFALL);
		if (zonktryslip(loc)) return;
		return;
	case TH_PREPAREFALL:
		if (zonktryfall(loc)) return;
		return;
	case TH_MOVEIN+TH_LEFT:
	case TH_MOVEIN+TH_RIGHT:
	case TH_MOVEIN+TH_DOWN:
		if (zonktryfall(loc)) return;
		if (zonkhit(loc,base)) return;
		if (zonktryslip(loc)) return;
		ldt[loc].set(base);
		return;
	default:
		ldt[loc].set(base);
		return;
	}
}

void display_zonk(int loc)
{
	GLfloat x,y;
	int rotate=0;
	computeloc(loc,y,x);
	if ((ldt[loc].typ & 0x0f) == TH_LEFT+TH_MOVEIN)
		rotate=animprogress(ldt[loc].counter,timing.zonkmove,spritecount(TT_zonkroll),0);
	if ((ldt[loc].typ & 0x0f) == TH_RIGHT+TH_MOVEIN)
		rotate=animprogress(ldt[loc].counter,timing.zonkmove,0,spritecount(TT_zonkroll));
	if (rotate==spritecount(TT_zonkroll)) rotate=0;
	painttex(y,x,rotate,TT_zonkroll,0);
}

void display_infotron(int loc)
{
	GLfloat x,y;
	int rotate=0;
	computeloc(loc,y,x);
	if ((ldt[loc].typ & 0x0f) == TH_LEFT+TH_MOVEIN)
		rotate=animprogress(ldt[loc].counter,timing.zonkmove,spritecount(TT_tronroll),0);
	if ((ldt[loc].typ & 0x0f) == TH_RIGHT+TH_MOVEIN)
		rotate=animprogress(ldt[loc].counter,timing.zonkmove,0,spritecount(TT_tronroll));
	if (rotate==spritecount(TT_tronroll)) rotate=0;
	painttex(y,x,rotate,TT_tronroll,0);
}



