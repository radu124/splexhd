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
#include "objConsole.h"
#include "objExplosion.h"

void touch_console()
{
	int i,j;
	for (i=lvlwidth+1; i<(lvlheight-1)*lvlwidth; i++)
	{
		if (ldt[i].xtyp()==TX_YELLOW) explode_region(i);
	}
};

void display_console(int loc)
{
};



