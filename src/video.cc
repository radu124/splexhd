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

#include "video.h"

#include "includes.h"
#include "globals.h"
#include "sprites.h"
#include "font.h"
#include "display.h"
#include "configuration.h"
#include "message.h"
#include "verbosity.h"

int windowres_x, windowres_y;

tVideoMode origres;

void videoToFullscreen()
{
	if (fullscreenmode) return;
	fullscreenmode=1;
	// =2 for the fake fullscreen mode
	windowres_x=scr_width;
	windowres_y=scr_height;
	//SDL_WM_ToggleFullScreen(scrsurface);
#ifdef _WINDOWS
	texReleaseAll();
	deffont.releaseAll();
#endif
	doResize(fsvideomode->width,fsvideomode->height);
#ifdef _WINDOWS
	texReloadAll();
	deffont.reloadAll();
#endif
}

void videoExitFullscreen()
{
	if (!fullscreenmode) return;
	fullscreenmode=0;
	//SDL_WM_ToggleFullScreen(scrsurface);
#ifdef _WINDOWS
	texReleaseAll();
	deffont.releaseAll();
#endif
	doResize(windowres_x, windowres_y);
#ifdef _WINDOWS
	texReloadAll();
	deffont.reloadAll();
#endif
}

void initviewport()
{
	glViewport( 0, 0, scr_width, scr_height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0.0f, scr_width, scr_height, 0.0f, -1.0f, 1.0f);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void doResize(int w, int h)
{
	scr_width=w;
	scr_height=h;
	scrsurface=SDL_SetVideoMode(scr_width, scr_height,
		((fullscreenmode==1)&&fsvideomode->bpp!=origres.bpp)?fsvideomode->bpp:0,
		SDL_HWSURFACE | SDL_OPENGL | SDL_RESIZABLE |
		((fullscreenmode==1)?SDL_FULLSCREEN:0));
	if (!scrsurface->flags & SDL_HWSURFACE)
		WARN(VIDEO,"No hardware surface resizing to %d x %d\n", scr_width, scr_height);
	initviewport();
	glEnable( GL_TEXTURE_2D );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	INFO(VIDEO,"RESIZE: %d %d\n", scr_width, scr_height);
}

void findvideomodes()
{
	tVideoMode tmp;
	int i;
	origmode=SDL_GetVideoInfo();
	tmp.bpp    =origmode->vfmt->BitsPerPixel;
	tmp.width  =origmode->current_w;
	tmp.height =origmode->current_h;
	videomodesdl=SDL_ListModes(NULL,SDL_FULLSCREEN|SDL_HWSURFACE);
	tmp.name="Desktop";
	origres=tmp;
	fsvideomode=new tVideoMode(tmp);
	videomodes.push_back(fsvideomode);
	if (videomodesdl==(void *)-1)
	{
		WARN(VIDEO,"ANY RESOLUTION ok\n");
		return;
	}
	if (videomodesdl) for (i=0; videomodesdl[i]; i++)
	{
		char a[32];
		tmp.width=videomodesdl[i]->w;
		tmp.height=videomodesdl[i]->h;
		INFO(VIDEO,"Mode:%d %dx%dx%d\n", i, tmp.width, tmp.height, tmp.bpp);
		sprintf(a,"%dx%dx%d", tmp.width, tmp.height, tmp.bpp);
		tmp.name=a;
		videomodes.push_back(new tVideoMode(tmp));
	}
}

void checkFullscreenRes()
{
	if (!video_dofullscreen) return;
	int i;
	for (i=0; i<videomodes.size(); i++)
	{
		if (scr_width==((tVideoMode *)videomodes[i])->width
		    && scr_height==((tVideoMode *)videomodes[i])->height)
			return;
	}
	video_dofullscreen=false;
}



