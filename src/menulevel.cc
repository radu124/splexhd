#include "menulevel.h"

#include "levelset.h"
#include "utils.h"
#include "gamedata.h"
#include "loadlevel.h"
#include "player.h"

void genmenulevel()
{
	int i,j;
	int n=getnumlevels();
	game.init();
	lvlwidth=20;
	lvlheight=3+2*n;
	dorealloc(ldt,lvlheight*lvlwidth+4);
	for (i=0; i<lvlwidth*lvlheight; i++) ldt[i].set(0);
	repairBorder();
	int portType=(TF_PORT | TB_PORTUPDN);
	int cntnf=0;
	for (i=0; i<n; i++)
	{
		if (levelstate(i)!=2)
		{
			cntnf++;
			if (cntnf>3) portType=TF_PORT | TB_PORTUP;
		}
		ldt[lvlwidth*(i*2+2)+1].set(portType);
		for (j=2; j<lvlwidth; j++) ldt[lvlwidth*(i*2+2)+j].set(TF_SOLID | TB_CONCRETE);
		ldt[lvlwidth*(i*2+3)+3].set(TX_INFOTRON);
		ldt[lvlwidth*(i*2+3)+4].set(TB_LEVTITLE);
	}
	game.murphloc=lvlwidth+1;
	if (game.lastlevel>0 && game.lastlevel<=n)
		game.murphloc=lvlwidth+1+game.lastlevel*2*lvlwidth;

	ldt[game.murphloc].set(TX_MURPH);
	game.mode=1;
	game.loadlevel=0;
}



