#ifndef __HDR_joystick_h
#define __HDR_joystick_h

#include "includes.h"

void init_joysticks();
void done_joysticks();
void joystick_initdetect();

struct tJoystick
{
	SDL_Joystick * dev;
	int ballsx[256];
	int ballsy[256];
	int axes[256];
	int hats[256];
};

extern int num_joysticks;
extern int joydetect_enable;
extern vector<tJoystick> joysticks;

#endif
