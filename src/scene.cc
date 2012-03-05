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
#include "scene.h"

#include "gamedata.h"

void scenehandleevent(SDL_Event &event)
{
	int downval=0;
	if (event.type==SDL_KEYDOWN) downval=1;
	if (event.type==SDL_KEYUP || event.type==SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_UP:     game.keydown[0]=downval; break;
		case SDLK_RIGHT:  game.keydown[1]=downval; break;
		case SDLK_DOWN:   game.keydown[2]=downval; break;
		case SDLK_LEFT:   game.keydown[3]=downval; break;
		case SDLK_SPACE:  game.keydown[4]=downval; break;
		case SDLK_ESCAPE: game.keydown[5]=downval; break;
		case SDLK_RETURN: game.keydown[6]=downval; break;
		}
	}
}



