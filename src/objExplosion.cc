#include "objExplosion.h"

#include "gamedata.h"
#include "timing.h"

void explode_region(int loc)
{
	int j;
	for (j=-1; j<2; j++)
	{
		explode_tile(above(loc)+j);
		explode_tile(loc+j);
		explode_tile(below(loc)+j);
	}
}

void explode_tile(int loc)
{
	int typ=ldt[loc].typ;
	if (ldt[loc].typ & TF_SOLID) return;
	ldt[loc].set(TX_VANISHING | TV_EXPLODE, timing.explosion);
	game.sfxexplodeleft=game.sfxexploderight=1;
}



