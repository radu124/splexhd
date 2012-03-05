#ifndef __HDR_globals_h
#define __HDR_globals_h


#include "includes.h"

// gave up namespaces

extern GLfloat scr_lrlim;
extern GLfloat scr_tblim;
extern string selectedsong;
extern int glfontaa;
extern GLint maxTexSize;
extern SDL_Surface* scrsurface;
extern int fullscreenmode;

extern mcolor C_WHITE;
extern mcolor C_BLACK;
extern mcolor C_TRANSPARENT;

extern SDL_Rect **videomodesdl;
extern const SDL_VideoInfo *origmode;
extern vstring videomodenames;

extern int gameover;
extern volatile int uu;


struct tConfigurable
{
	int idx;
	string name;
};

struct tVideoMode:public tConfigurable
{
	int height,width,bpp;
};

extern vector<tConfigurable*> videomodes;
extern tVideoMode *fsvideomode;

/**
 * level scroll within screen
 */
extern int scrdx;
extern int scrdy;

/**
 * tile size
 */
extern GLfloat ts;

#endif
