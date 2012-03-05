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

#include "globals.h"

mcolor C_WHITE(1,1,1,1);
mcolor C_BLACK(0,0,0,1);
mcolor C_TRANSPARENT(0,0,0,1);

int gameover;
volatile int uu;

SDL_Rect **videomodesdl;
const SDL_VideoInfo *origmode;
vstring videomodenames;

GLfloat scr_lrlim;
GLfloat scr_tblim;
string selectedsong="";
int glfontaa=GL_LINEAR;
GLint maxTexSize;
SDL_Surface* scrsurface;
int fullscreenmode=0;

vector<tConfigurable*> videomodes;
tVideoMode *fsvideomode;

int scrdx;
int scrdy;
GLfloat ts=64;
