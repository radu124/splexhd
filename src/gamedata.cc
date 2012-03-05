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
#include "gamedata.h"

ttile *ldt=NULL;
int ldt_allocd=0;
string levelname;
tdemo demo;
int lvlwidth=60;  /// width of a level
int lvlheight=24; /// height of a level
tgamestate game;

void ttile::set(int vtyp, int vcounter, int vexplode)
{
	typ=vtyp;
	counter=vcounter;
	xplode=vexplode;
}

void tdemo::init()
{
}

void tgamestate::init()
{
	mode=GAMEMODE_NORMAL;
	murphloc=0;
	victory=0;
	ended=0;
	infotronsneeded=0;
	loadlevel=0;
	murphlastleft=0;
	murphsdinner=0;
	reddiskcount=0;
	reddisktimeout=0;
	reddisklocation=0;
	timestarted=SDL_GetTicks();
	timelastmove=timestarted;
	freezeenemies=0;
}



