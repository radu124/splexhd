#include "objKnik.h"

#include "gamedata.h"
#include "display.h"
#include "timing.h"
#include "play.h"
#include "objExplosion.h"
#include "sprites.h"

void doobj_knik(int loc)
{
	int typ=ldt[loc].typ;
	int base=typ & 0xf0;
	int thh=typ & 0x0c;
	int thd=typ & 0x03;
	int AHEAD, LEFT, RIGHT;
	if (ldt[loc].counter) return;
	// set ahead and left from the point of view of the enemy
	switch (thd)
	{
	case TH_UP:
		AHEAD=above(loc);
		LEFT=leftof(loc);
		RIGHT=rightof(loc);
		break;
	case TH_RIGHT:
		AHEAD=rightof(loc);
		LEFT=above(loc);
		RIGHT=below(loc);
		break;
	case TH_DOWN:
		AHEAD=below(loc);
		LEFT=rightof(loc);
		RIGHT=leftof(loc);
		break;
	default: // TH_LEFT
		AHEAD=leftof(loc);
		LEFT=below(loc);
		RIGHT=above(loc);
	}
	if (ldt[AHEAD].xtyp()==TX_MURPH)
	{
		game.ended=1;
		explode_region(AHEAD);
		return;
	}
	switch (thh)
	{
	case TH_ANTICLOCK:
		if (ldt[AHEAD].isfreeormurph())
		{
			// move ahead
			ldt[AHEAD].set(base | TH_MOVEIN | thd, timing.enemymove);
			ldt[loc].set(TX_MOVEOUT | thd, timing.enemymove);
		} else {
			ldt[loc].set(base | TH_ANTICLOCK | ((thd-1)&3), timing.enemyrotate);
		}
		return;
	case TH_CLOCKWISE:
		if (ldt[AHEAD].isfreeormurph())
		{
			// move ahead
			ldt[AHEAD].set(base | TH_MOVEIN | thd, timing.enemymove);
			ldt[loc].set(TX_MOVEOUT | thd, timing.enemymove);
		} else {
			ldt[loc].set(base | TH_CLOCKWISE | ((thd+1)&3), timing.enemyrotate);
		}
		return;
	default: // MOVE, or INVALID
		if (ldt[AHEAD].isfreeormurph() && !ldt[LEFT].isfreeormurph())
		{
			// move ahead
			ldt[AHEAD].set(base | TH_MOVEIN | thd, timing.enemymove);
			ldt[loc].set(TX_MOVEOUT | thd, timing.enemymove);
			return;
		}
		if (ldt[RIGHT].isfreeormurph() && !ldt[LEFT].isfreeormurph())
		{
			// turn clockwise
			ldt[loc].set(base | TH_CLOCKWISE | ((thd+1)&3), timing.enemyrotate);
			return;
		}
		// default is same as left-turn
		ldt[loc].set(base | TH_ANTICLOCK | ((thd-1)&3), timing.enemyrotate);
		return;
	}
}

void display_knik(int loc)
{
	GLfloat x,y;
	computeloc(loc,y,x);
	int rotation=0;
	int typ=ldt[loc].typ;
	int thh=typ & 0x0c;
	int thd=typ & 0x03;
	if (thh==TH_ANTICLOCK) rotation=thd*90+animprogress(ldt[loc].counter,timing.enemyrotate,90);
	if (thh==TH_CLOCKWISE) rotation=thd*90-animprogress(ldt[loc].counter,timing.enemyrotate,90);
	if (thh==TH_MOVEIN) rotation=thd*90;
	painttex(y,x,0,TT_knikcut,rotation);
}

void display_electron(int loc)
{
	GLfloat x,y;
	computeloc(loc,y,x);
	painttex(y,x,getelectronid(game.timenow-game.timestarted),TT_electron,0);
}

void initobj_knik(int loc)
{
	int base=ldt[loc].typ & 0xf0;
	int ABOVE=above(loc);
	int LEFTOF=leftof(loc);
	int RIGHTOF=rightof(loc);
	ldt[loc].set(base | TH_ANTICLOCK | TH_LEFT, timing.enemymove);
	if (ldt[LEFTOF].isfree())
	{
		ldt[loc].counter=timing.enemymoveshort;
		return;
	}
	if (ldt[ABOVE].isfree())
	{
		ldt[ABOVE].set(base | TH_UP | TH_MOVEIN, timing.enemymove);
		ldt[loc].set(TX_MOVEOUT | TH_UP,timing.enemymove);
		return;
	}
	if (ldt[RIGHTOF].isfree())
	{
		ldt[RIGHTOF].set(base | TH_RIGHT | TH_MOVEIN, timing.enemymove);
		ldt[loc].set(TX_MOVEOUT | TH_RIGHT, timing.enemymove);
	}
}



