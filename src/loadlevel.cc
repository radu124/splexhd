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
#include "loadlevel.h"
#include "gamedata.h"
#include "objKnik.h"
#include "utils.h"
#include "timing.h"

int loadLevel(const uint8_t* levdat)
{
	int i;
	int levdsize;
	int levfile;
	// find murphy
	game.init();
	lvlwidth=60;
	lvlheight=24;
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++) if (levdat[i]==TB_MURPHY)
		{ game.murphloc=i; break; }
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++) if (levdat[i]==TB_INFOTRON)
		game.infotronsneeded++;
	if (levdat[1470]) game.infotronsneeded=levdat[1470];
	game.gravity=levdat[1444]&1;
	// mo murphy
	if (!game.murphloc)
	{
		game.murphloc=lvlwidth+1;
	}
	dorealloc(ldt,lvlheight*lvlwidth+4);
	for (i=0; i<lvlheight*lvlwidth; i++)
	{
		ldt[i].set(levdat[i]);
		switch ((unsigned char) levdat[i])
		{
		case 0x05:
		case 0x1a:
		case 0x1b:
		case 0x26:
		case 0x27: ldt[i].set(TF_IC | levdat[i]); break; // ics
		case 0x06:
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x1f:
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25: ldt[i].set(TF_SOLID | levdat[i]); break; // solids

		case 0x02: ldt[i].set(TB_PCB); break;
		case 0x03: ldt[i].set(TB_MURPHY); break;
		case 0x04: ldt[i].set(TX_INFOTRON); break;

		case 0x00: break;// empty

		// these are special but there's no reason to treat them specially
		// at this point, they are destructible like ICs
		case 0xff: ldt[i].set(TF_IC); break;                 // ?
		case 0x07:                                           // exit
		case 0x13: ldt[i].set(TF_IC | levdat[i]); break;     // detonator

		case 0x01: ldt[i].set(TX_ZONK); break;         // zonk
		case 0x08: ldt[i].set(TX_ORANGE); break;      // orange disk
		case 0x12: ldt[i].set(TX_YELLOW); break; // yellowdisk
		case 0x11: ldt[i].set(TX_KNIK); break;  // scissors
		case 0x18: ldt[i].set(TX_ELECTRON); break;  // electron

		case 0x09: ldt[i].set(TF_PORT | levdat[i]); break; // port >
		case 0x0a: ldt[i].set(TF_PORT | levdat[i]); break; // port V
		case 0x0b: ldt[i].set(TF_PORT | levdat[i]); break; // port <
		case 0x0c: ldt[i].set(TF_PORT | levdat[i]); break; // port ^
		case 0x14: ldt[i].set(TF_PORT | levdat[i]); break; // reddisk
		case 0x15: ldt[i].set(TF_PORT | levdat[i]); break; // port ud
		case 0x16: ldt[i].set(TF_PORT | levdat[i]); break; // port lr
		case 0x17: ldt[i].set(TF_PORT | levdat[i]); break; // port all
		case 0x0d: ldt[i].set(TD_PORTSPECIAL + levdat[i]-4); break; // special port >
		case 0x0e: ldt[i].set(TD_PORTSPECIAL + levdat[i]-4); break; // special port V
		case 0x0f: ldt[i].set(TD_PORTSPECIAL + levdat[i]-4); break; // special port <
		case 0x10: ldt[i].set(TD_PORTSPECIAL + levdat[i]-4); break; // special port ^

		case 0x19: ldt[i].set(TB_PCB | TF_BUG,timing.bugact); break; // bug
		default: ldt[i].set(0);
		}
	}
	ldt[game.murphloc].set(TX_MURPH);
	repairBorder();
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++)
		if (ldt[i].isenemy())
			initobj_knik(i);

	demo.init();
	return 1;
}

void repairBorder()
{
	int i;
	for (i=0; i<lvlwidth; i++)
	{
		ldt[i].set(TF_SOLID | TB_CONCRETE);
		ldt[i+(lvlheight-1)*lvlwidth].set(TF_SOLID | TB_CONCRETE);
	}
//	for (i=0; i<49; i++) ldt[i+lvlwidth+1].set(i);
	for (i=1; i<lvlheight; i++)
	{
		ldt[i*lvlwidth].set(TF_SOLID | TB_CONCRETE);
		ldt[i*lvlwidth+lvlwidth-1].set(TF_SOLID | TB_CONCRETE);
	}
}



