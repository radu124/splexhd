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
#include "objYellow.h"

#include "gamedata.h"
#include "display.h"
#include "sprites.h"

void display_yellow(int loc)
{
	GLfloat x,y;
	computeloc(loc,y,x);
	painttex(y,x,TB_YELLOW,TT_tiles,0);
}



