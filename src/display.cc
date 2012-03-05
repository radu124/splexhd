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

#include "display.h"
#include "gamedata.h"
#include "levelset.h"
#include "player.h"
#include "status.h"
#include "timing.h"
#include "sprites.h"
#include "play.h"
#include "configuration.h"
#include "font.h"

#include "objMurph.h"
#include "objKnik.h"
#include "objZonk.h"
#include "objOrange.h"
#include "objYellow.h"
#include "objVanishing.h"

void computeloc(int loc, GLfloat &locy, GLfloat &locx, int flags)
{
	locy=(loc/lvlwidth)*ts-scrdy;
	locx=(loc%lvlwidth)*ts-scrdx;
	int typ=ldt[loc].typ & 0xff;
	if (typ<0x40) return;
	if (flags&1) return;
	int disp=animprogress(ldt[loc].counter,timing.o8,-ts);
	if (((typ & 0x0c) == TH_MOVEDOUBLE) && ((typ & 0xf0)==TX_MURPH)) typ-=4;
	switch (typ & 0x0f)
	{
	case TH_MOVEIN + TH_UP:
		locy-=disp;
		return;
	case TH_MOVEIN + TH_DOWN:
		locy+=disp;
		return;
	case TH_MOVEIN + TH_LEFT:
		locx-=disp;
		return;
	case TH_MOVEIN + TH_RIGHT:
		locx+=disp;
		return;
	}
}

void painttex(GLfloat i, GLfloat j, int id, int group, int rot, int flags)
{
	int texid;
	if (!id && !group) return;
	GLfloat texl=0, texr=1.0;
	GLfloat sp_width=ts,sp_height=ts;
	texid=gettex(group,id);
	if (texid!=0) glBindTexture(GL_TEXTURE_2D,texid);
	else {
		glBindTexture(GL_TEXTURE_2D,0);
		glColor4f(1,0,0,1);
	}
	glPushMatrix();
	glTranslatef(j+ts*0.5,i+ts*0.5,0);
	if (rot)
	{
		glRotatef(rot,0,0,1);
	}
	if (flags & 1)
	{
		texl=1.0;
		texr=0;
	}
	if (flags & 2)
	{
		sp_width=ts*0.8;
		sp_height=ts*1.4;
	}
	sp_width*=0.5;
	sp_height*=0.5;
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texl , 0.0); glVertex3f(-sp_width,-sp_height, 0);
	glTexCoord2f(texr , 0.0); glVertex3f( sp_width,-sp_height, 0);
	glTexCoord2f(texl , 1.0); glVertex3f(-sp_width, sp_height, 0);
	glTexCoord2f(texr , 1.0); glVertex3f( sp_width, sp_height, 0);
	glEnd();
	glPopMatrix();
	glColor4f(1,1,1,1);
	glBindTexture(GL_TEXTURE_2D,0);
}

void renderscene()
{
	int i,j;

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	game.timenow=SDL_GetTicks();
	if (ldt==NULL) return;
	for (i=0; i<lvlheight*lvlwidth; i++)
	{
		int typ=ldt[i].typ;
		int tf=typ & 0xff;
		int painttyp=tf;

		if (tf>=0x40) painttyp=0;
		//if ((tf&0xf0)==TX_KNIK) painttyp=TB_INFOTRON;
		if ((tf&0xf0)==0xf0)
			painttyp=TB_MYTILE+(tf&3);

		GLfloat x,y;
		computeloc(i,y,x);
		if (typ==TB_LEVTITLE)
		{
			if (y<-ts || y>scr_height) continue;
			int lnum=i/lvlwidth/2-1;
			int st=levelstate(lnum);
			if (st==0) glColor4f(1,0,0,1);
			else if (st==1) glColor4f(1,1,0,1);
			else glColor4f(0,1,0,1);
			painttyp=0;
			deffont.displayString(getlevelname(lnum).c_str(),x,y+ts*0.9,ts);
			glColor4f(1,1,1,1);
		}
		else if (painttyp) painttex(y,x,painttyp);
	}
	for (i=0; i<lvlheight*lvlwidth; i++)
	{
		int tx=ldt[i].xtyp();
		if (tx==TX_KNIK      ) display_knik(i);
	    if (tx==TX_ELECTRON  ) display_electron(i);
	    if (tx==TX_MURPH     ) display_murphy(i);
		if (tx==TX_ZONK      ) display_zonk(i);
		if (tx==TX_INFOTRON  ) display_infotron(i);
		if (tx==TX_ORANGE    ) display_orange(i);
		if (tx==TX_YELLOW    ) display_yellow(i);
		if (tx==TX_VANISHING ) display_vanishing(i);
	}
	display_status();
}



