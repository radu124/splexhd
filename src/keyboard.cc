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

#include "keyboard.h"

#include "configuration.h"
#include "includes.h"
#include "configuration.h"
#include "joystick.h"
#include "scene.h"

string keyname(int id)
{
	char a[32];
	if (id<1000000) return SDL_GetKeyName((SDLKey) id);
	if (id<2000000)
	{
		id-=1000000;
		sprintf(a,"Joy #%d %c",id/256,'a'+id%256);
	} else if (id<3000000) {
		id-=2000000;
		sprintf(a,"Joy #%d hat%d%c,",id/1024,(id&1023)/4,"NSWE"[id&3]);
	} else if (id<4000000) {
		id-=3000000;
		sprintf(a,"Joy #%d axis%d",id/256,id%256);
	} else if (id<5000000) {
		id-=4000000;
		sprintf(a,"Joy #%d ball%d%c",id/512,(id&511)/2,"XY"[id&1]);
	} else return "config err";
	return a;
}

// perform translation before sending to scene
void translateandhandleevent(SDL_Event &event)
{
	SDL_Event e=event;
	
	// translate hat to buttons
	// a single hat event may generate multiple button events
	if (e.type==SDL_JOYHATMOTION)
	{
		int joy=e.jhat.which;
		int hat=e.jhat.hat;
		int hval=e.jhat.value;
		int prev=joysticks[joy].hats[hat];
		int base=2000000+joy*1024+hat*4;
		e.type=SDL_KEYDOWN;
		if ((hval & SDL_HAT_UP   ) && !(prev & SDL_HAT_UP   )) { e.key.keysym.sym=(SDLKey) base;     scenehandleevent(e); }
		if ((hval & SDL_HAT_DOWN ) && !(prev & SDL_HAT_DOWN )) { e.key.keysym.sym=(SDLKey) (base+1); scenehandleevent(e); }
		if ((hval & SDL_HAT_LEFT ) && !(prev & SDL_HAT_LEFT )) { e.key.keysym.sym=(SDLKey) (base+2); scenehandleevent(e); }
		if ((hval & SDL_HAT_RIGHT) && !(prev & SDL_HAT_RIGHT)) { e.key.keysym.sym=(SDLKey) (base+3); scenehandleevent(e); }
		e.type=SDL_KEYUP;
		if (!(hval & SDL_HAT_UP   ) && (prev & SDL_HAT_UP   )) { e.key.keysym.sym=(SDLKey) base;     scenehandleevent(e); }
		if (!(hval & SDL_HAT_DOWN ) && (prev & SDL_HAT_DOWN )) { e.key.keysym.sym=(SDLKey) (base+1); scenehandleevent(e); }
		if (!(hval & SDL_HAT_LEFT ) && (prev & SDL_HAT_LEFT )) { e.key.keysym.sym=(SDLKey) (base+2); scenehandleevent(e); }
		if (!(hval & SDL_HAT_RIGHT) && (prev & SDL_HAT_RIGHT)) { e.key.keysym.sym=(SDLKey) (base+3); scenehandleevent(e); }
		joysticks[joy].hats[hat]=hval;
		return;
	}
	if (e.type==SDL_JOYBUTTONDOWN)
	{
		e.type=SDL_KEYDOWN;
		e.key.keysym.sym=(SDLKey) (1000000+256*e.jbutton.which+e.jbutton.button);
	}
	if (e.type==SDL_JOYBUTTONUP)
	{
		e.type=SDL_KEYUP;
		e.key.keysym.sym=(SDLKey) (1000000+256*e.jbutton.which+e.jbutton.button);
	}
	scenehandleevent(e);
}

void menukey(SDL_Event &event, int flags)
{
	if (event.type!=SDL_KEYDOWN && event.type!=SDL_KEYUP) return;
	switch (event.key.keysym.sym)
	{
		// these keys are never remapped
		case SDLK_ESCAPE:
		case SDLK_RETURN:
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
		case SDLK_HOME:
		case SDLK_END:
		case SDLK_PAGEUP:
		case SDLK_PAGEDOWN:
		return;
	}
}



