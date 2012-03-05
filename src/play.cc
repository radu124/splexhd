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
#include "play.h"

#include "gamedata.h"
#include "timing.h"
#include "loadlevel.h"
#include "levelset.h"
#include "menulevel.h"
#include "player.h"
#include "audiosfx.h"
#include "effects.h"

#include "objBug.h"
#include "objConsole.h"
#include "objExplosion.h"
#include "objKnik.h"
#include "objOrange.h"
#include "objZonk.h"
#include "objMurph.h"
#include "objVanishing.h"

int lvlshowdispi=0;
int lvlshowdispj=0;
int lastframetime=0;
int thisframetime;
int tcnti,tcntj;
int relyonrefresh=0;
int slomo=0;
int slocount=20;
int expectedtimeperframe=5000;


uint32_t timenow=0;
uint32_t timestarted=0;
uint32_t showlevstat=3;

uint64_t timerfreq=0;

uint32_t gettimeuseconds();
int appIdle();
int getdemomove();
void startgame();
void gamekeypressed(int);
void endgame();

int animprogress(int counter, int maxcounter, int animstart, int animstop)
{
	return animstart+(animstop-animstart)*(maxcounter-counter)/maxcounter;
}

int getdemomove()
{
	if (demo.frame>=timing.m1) demo.frame=0;
	// if it's one of the 8 times that movement is sampled
	if (timing.isoct(demo.frame))
	{
		if (!demo.mcnt)
		{
			demo.move=demo.dat[demo.pos++];
			if (demo.move==0xff)
			{
				demo.length=0;
				return 0;
			}
			demo.mcnt=(demo.move>>4)+1;
			demo.move&=0x0f;
		}
		demo.mcnt--;
	}
	demo.frame++;
	if (demo.move>9) return 0;
	return demo.move;
}

void startgame()
{
	load_levelset("data/levels/levels.dat");
	//if (!loadLevel(getleveldata(0))) return;
	genmenulevel();
}

void endgame()
{
}

void renderframe()
{
}

int appIdle()
{
/*
	if (menumode || !appActive) return 1;

	//lastframetime=gettimeuseconds();
	renderframe();
	thisframetime=gettimeuseconds();
	timenow=gettimeuseconds();
	if (gamemode==GAMEMODE_NORMAL)
	{
		if ((thisframetime-lastframetime)>200000) thisframetime=lastframetime+10000;
		if (relyonrefresh) playGame();
		else while ((thisframetime-lastframetime)>6000)
		{
			lastframetime+=expectedtimeperframe;
			playGame();
		}
	}
*/
	return 0;
}

void playGame()
{
	/*
	if (slomo)
	{
		if (slocount-- >0) return;
		slocount=80;
	}
	if (gamelost&&(!endgamecountdown)) endgamecountdown=100;
	if (endgamecountdown)
	{
		endgamecountdown--;
		if (!endgamecountdown)
		{
			switchtomenumode();
			return;
		}
	}
	if (moviefile!=-1)
	{
		ldt[0].b.extra=lastactionkey;
		_write(moviefile,ldt,sizeof(ldt));
		ldt[0].b.extra=0;
	}
	doMurphy();
	zoomOnMurphy();
	*/
	if (game.loadlevel)
	{
		if (game.loadlevel<0)
		{
			// this should only happen on a level exit
			if (game.lastlevel>0 && game.mode==0)
				finishlevel(game.lastlevel-1,game.victory,game.timenow-game.timestarted);
			genmenulevel();
		}
		else if (!loadLevel(getleveldata(game.loadlevel-1))) genmenulevel();
	}
	if (game.timenow-game.timelastmove>30)
	{
		doObjects();
		doobj_murphy(game.murphloc);
		zoomon_murphy(game.murphloc);
		if (game.sfxexplodeleft || game.sfxexploderight)
		{
			sfx_start(SE_explosion);
			game.sfxexplodeleft=game.sfxexploderight=0;
		}
		if (game.sfxzonkleft || game.sfxzonkright)
		{
			sfx_start(SE_hit);
			game.sfxzonkleft=game.sfxzonkright=0;
		}

	}
}



void doObjects()
{
	game.timelastmove=game.timenow;
	int i,typ,tb,tx;
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++)
	{
		ldt[i].oldtyp=ldt[i].typ;
	}
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++)
	{
		int cnt=ldt[i].counter;
		if (cnt && (ldt[i].xtyp()==(ldt[i].oldtyp & 0xf0))) ldt[i].counter=--cnt;
		typ=ldt[i].typ;
		tx=typ & 0xf0;
		tb=typ & 0xff;
		// call the appropriate function based on object time
		switch (tx)
		{
		case TX_MOVEOUT:
			if (!cnt) ldt[i].set(0);
			break;
		case TX_ORANGE:
			doobj_orange(i);
			break;
		case TX_ZONK: // zonk and infotron behave similarly
		case TX_INFOTRON:
			doobj_zonk(i);
			break;
		case TX_KNIK:
		case TX_ELECTRON:
			doobj_knik(i);
			break;
		case TX_VANISHING:
			doobj_vanishing(i);
			break;
		}
	}
}



