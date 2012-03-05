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

#include "joystick.h"
#include "message.h"
#include "verbosity.h"

int num_joysticks;
int joydetect_enable;
vector<tJoystick> joysticks;

void init_joysticks()
{
	int i;
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	num_joysticks=SDL_NumJoysticks();
	MESSAGE("Joysticks: %d\n", num_joysticks);
	for (i=0; i<num_joysticks; i++)
	{
		tJoystick temp;
		temp.dev=SDL_JoystickOpen(i);
		if (!temp.dev) continue;
		joysticks.push_back(temp);
	}
	SDL_JoystickEventState(SDL_ENABLE);
}

void done_joysticks()
{
	int i;
	for (i=0; i<joysticks.size(); i++)
		SDL_JoystickClose(joysticks[i].dev);
}

void joystick_initdetect()
{
	// set up joystick detection
	// we will respond to the first major axis event
	// and transform it into a keystroke
	joydetect_enable=1;
	int i,j;
	for (i=0; i<joysticks.size(); i++)
		for (j=0; j<256; j++) joysticks[i].axes[j]=-1000000;
}



