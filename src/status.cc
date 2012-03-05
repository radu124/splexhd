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
#include "status.h"

#include "includes.h"
#include "sprites.h"
#include "gamedata.h"
#include "configuration.h"
#include "display.h"

void display_status()
{
	int posx=scr_width-ts;
	int x=game.infotronsneeded;
	int digitnr=0;
	while (x>0)
	{
		painttex(ts,scr_width-ts*0.7*digitnr-ts, x%10, TT_digits, 0, 2);
		x/=10;
		digitnr++;
	}
}



