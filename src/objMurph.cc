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
#include "objMurph.h"

#include "includes.h"
#include "gamedata.h"
#include "audio.h"
#include "effects.h"
#include "objConsole.h"
#include "video.h"
#include "objExplosion.h"
#include "timing.h"
#include "audiosfx.h"
#include "play.h"
#include "display.h"
#include "sprites.h"
#include "configuration.h"

int ttile::isdeadly()
{
	int tx=xtyp();
	int th=movfull();
	return (tx==TX_MOVEOUT && movdirection()!=TH_LEFT && movdirection()!=TH_RIGHT)
		|| (tx==TB_INFOTRON && th!=0)
//		|| (tx==TX_ZONK && th!=0)
		|| tx==TX_KNIK
		|| tx==TX_ELECTRON;

}

/**
 * Try moving into a tile by eating its contents
 * (works even when the tile is empty)
 * @param loc - the location of Murphy
 * @param i - direction of movement
 * @param dest - destination tile
 * @return 1 on success
 */
int tryeating(int loc, int i, int dest)
{
	int result=ldt[dest].ispcb() || ldt[dest].isfree() || ldt[dest].xtyp()==TX_INFOTRON || ldt[dest].basetyp()==TB_REDDISK;
	if (ldt[dest].isdeadly())
	{
		game.ended=1;
		explode_region(loc);
		return 1;
	}
	if (!result) return 0;
	game.murphsdinner=ldt[dest].basetyp();
	ldt[dest].set(TX_MURPH + TH_MOVEIN + i, timing.murphmove);
	ldt[loc].set(TX_MOVEOUT + i, timing.murphmove);
	if ((game.murphsdinner&0xf0)==TX_INFOTRON)
	{
		sfx_start(SE_infotron);
		if (game.infotronsneeded) game.infotronsneeded--;
		game.murphsdinner=TB_INFOTRON;
		if (game.mode==1) game.lastlevel=game.loadlevel=(loc/lvlwidth)/2;
	}
	if (game.murphsdinner==TB_PCB)
	{
		sfx_start(SE_pcb);
	}
	if (game.murphsdinner==TB_REDDISK) game.reddiskcount++;
	game.murphloc=dest;
	if (game.murphsdinner>=0x40) game.murphsdinner=0;
	if (i==1) game.murphlastleft=0;
	if (i==3) game.murphlastleft=1;
	return 1;
}

/**
 * Try moving through a port, the cell at the other side of the port
 * must be empty
 * @param loc - the location of Murphy
 * @param i - direction of movement
 * @param dest - port tile
 * @return 1 on success
 */
int tryport(int loc, int i, int dest)
{
	int fardest=dest*2-loc; // amplify delta
	if (i==0 && !ldt[dest].isportup()) return 0;
	if (i==1 && !ldt[dest].isportright()) return 0;
	if (i==2 && !ldt[dest].isportdown()) return 0;
	if (i==3 && !ldt[dest].isportleft()) return 0;
	if (!ldt[fardest].isfree()) return 0;
	game.murphloc=fardest;
	ldt[loc].set(TX_MOVEOUT | i,timing.murphmove);
	ldt[fardest].set(TX_MURPH | TH_MOVEDOUBLE | i,timing.murphmove);
	if (i==1) game.murphlastleft=0;
	if (i==3) game.murphlastleft=1;
}

/**
 * Try grabbing the contents of an adjacent tile
 * (works with pcb, red disks, and infotrons)
 * @param loc - the location of Murphy
 * @param i - direction of movement
 * @param dest - destination tile
 * @return 1 on success
 */
int trygrab(int loc, int i, int dest)
{
	if (ldt[dest].ispcb())
	{
		// mind the bugs
		ldt[dest].set(TX_VANISHING | TB_PCB, timing.zonkmove);
		sfx_start(SE_pcb);
		return 1;
	}
	if (ldt[dest].basetyp()==TX_INFOTRON)
	{
		// only for infotrons at rest
		ldt[dest].set(TX_VANISHING | TB_INFOTRON, timing.zonkmove);
		sfx_start(SE_infotron);
		if (game.infotronsneeded) game.infotronsneeded--;
		return 1;
	}
	if (ldt[dest].basetyp()==TB_REDDISK)
	{
		ldt[dest].set(TX_VANISHING | TV_REDDISK, timing.zonkmove);
		return 1;
	}
	return 0;
}

/**
 * Try pushing the contents of a tile
 * (works with zonks, orange disks and yellow disks,
 * yellow disks can also be pushed vertically)
 * @param loc - the location of Murphy
 * @param i - direction of movement
 * @param dest - destination tile
 * @return 1 on success
 */
int trypush(int loc, int i, int dest)
{
	// pushing should actually be done in two stages
	// we ignore that for now
	int typ=ldt[dest].basetyp();
	int fardest=dest*2-loc; // amplify displacement
	if (typ!=TX_YELLOW && (i==TH_UP || i==TH_DOWN)) return 0;
	if (typ!=TX_YELLOW && typ!=TX_ORANGE && typ!=TX_ZONK) return 0;
	if (!ldt[fardest].isfree()) return 0;
	ldt[fardest].set(typ | TH_MOVEIN | i,timing.zonkmove);
	ldt[dest].set(TX_MURPH | TH_MOVEIN | i,timing.zonkmove);
	ldt[loc].set(TX_MOVEOUT | i, timing.zonkmove);
	game.murphloc=dest;
	return 1;
}

/*
 * This is where the player actions are handled
 * very important
 *
 * actions, tested in this order can be
 * touch or run into EXIT
 * touch or run into CONSOLE
 * grab: PCB, REDDISK or Infotron
 */
void doobj_murphy(int loc)
{
	int dest[4];
	int i;
	if (game.ended)
	{
		if (ldt[loc].counter==0) game.loadlevel=-1;
		return;
	}
	if (game.keydown[5])
	{
		if (game.mode==1) gameover=1;
		game.ended=1;
		explode_region(loc);
	}
	if (ldt[loc].counter>0) return;
	dest[0]=above(loc);
	dest[1]=rightof(loc);
	dest[2]=below(loc);
	dest[3]=leftof(loc);
	game.murphsdinner=0;
	ldt[loc].set(TX_MURPH);
	for (i=0; i<4; i++) if (game.keydown[i])
	{
		if (ldt[dest[i]].basetyp()==TB_EXIT && !game.infotronsneeded)
		{
			sfx_start(SE_victory);
			game.ended=1;
			game.victory=1;
			ldt[loc].set(TX_VANISHING | TB_MURPHY,timing.murphvanishing);
			return;
		}
		if (ldt[dest[i]].basetyp()==TB_CONSOLE)
		{
			game.murphlookat=i;
			touch_console();
			return;
		}
		if (game.keydown[4])
		{
			trygrab(loc,i,dest[i]);
			game.murphlookat=i;
			return;
		}
		if (tryeating(loc,i,dest[i])) return;
		if (tryport(loc,i,dest[i])) return;
		if (trypush(loc,i,dest[i])) return;
	}
};

void display_murphy(int loc)
{
	int murphprogress=animprogress(ldt[loc].counter,timing.murphmove,0,7);
	GLfloat x,y;
	int t;
	if (ldt[loc].movfull()==TH_VANISHING)
	{
		computeloc(loc,y,x);
		murphprogress=animprogress(ldt[loc].counter,timing.murphvanishing,0,spritecount(TT_murphvanishing)-1);
		painttex(y,x,murphprogress,TT_murphvanishing,0);
		return;
	}
	if (ldt[loc].movtyp()==TH_MOVEIN && game.murphsdinner)
	{
		computeloc(loc,y,x,1);
		painttex(y,x,game.murphsdinner,TT_tiles,0);
	}
	computeloc(loc,y,x);
	painttex(y,x,murphprogress,TT_murphwalk,0,game.murphlastleft);
	// Murph is passing through a port, draw murph twice and the port over him
	if (ldt[loc].movtyp()==TH_MOVEDOUBLE)
	{
		switch(ldt[loc].movdirection())
		{
			case TH_UP:
				painttex(y+ts,x,murphprogress,TT_murphwalk,0,game.murphlastleft);
				t=below(loc);
				computeloc(t,y,x);
				painttex(y,x,ldt[t].basetyp());
				break;
			case TH_DOWN:
				painttex(y-ts,x,murphprogress,TT_murphwalk,0,game.murphlastleft);
				t=above(loc);
				computeloc(t,y,x);
				painttex(y,x,ldt[t].basetyp());
				break;
			case TH_LEFT:
				painttex(y,x+ts,murphprogress,TT_murphwalk,0,game.murphlastleft);
				t=rightof(loc);
				computeloc(t,y,x);
				painttex(y,x,ldt[t].basetyp());
				break;
			case TH_RIGHT:
				painttex(y,x-ts,murphprogress,TT_murphwalk,0,game.murphlastleft);
				t=leftof(loc);
				computeloc(t,y,x);
				painttex(y,x,ldt[t].basetyp());
				break;
		}
	}
};

void zoomon_murphy(int loc)
{
	GLfloat locx,locy;
	if (ldt[loc].xtyp()!=TX_MURPH) return;
	locx=ts * (loc%lvlwidth);
	locy=ts * (loc/lvlwidth);
	int delta=animprogress(ldt[loc].counter,timing.murphmove,ts*2,0);
	if (ldt[loc].movtyp()==TH_MOVEIN) delta=trunc(delta/2);
	else if (ldt[loc].movtyp()!=TH_MOVEDOUBLE) delta=0;
	switch (ldt[loc].movdirection())
	{
	case TH_UP:    locy+=delta; break;
	case TH_DOWN:  locy-=delta; break;
	case TH_LEFT:  locx+=delta; break;
	case TH_RIGHT: locx-=delta; break;
	}
	scrdx=locx-trunc(scr_width/2-ts/2);
	scrdy=locy-trunc(scr_height/2-ts/2);
	if (game.forcecenter) return;
	scrdx=max(0.0f,min((GLfloat) scrdx,ts*(lvlwidth+1)-scr_width));
	scrdy=max(0.0f,min((GLfloat) scrdy,ts*(lvlheight)-scr_height));
}



