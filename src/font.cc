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

#include "font.h"

#include "message.h"
#include "verbosity.h"
#include "globals.h"

FT_Library font_library;
TFont deffont;

void TFont::displayString(const char *str,GLfloat x, GLfloat y, GLfloat scale, int pos)
{
	GLfloat disp=0;
	if (pos!=0)
	{
		const char *sc=str;
		while (sc[0])
		{
			unsigned char ch=*sc++;
			if (!texvalid[ch]) ch='?';
			disp+=scale*cadvance[ch];
		}
		if (pos & ALIGN_CENTER) x-=disp/2;
		if (pos & ALIGN_RIGHT) x-=disp;
		if (pos & ALIGN_VONETHIRD) y+=scale/3.0f*cbaseline['A'];
		if (pos & ALIGN_VHALFA) y+=scale/2.0f*cbaseline['A'];
	}
	while (str[0])
	{
		unsigned char ch=*str++;
		if (!texvalid[ch]) ch='?';
		glBindTexture(GL_TEXTURE_2D,textures[ch]);
		glBegin(GL_QUADS);
		GLfloat dx=x+scale*cxdisp[ch];
		GLfloat dy=y-scale*cbaseline[ch];
		//glTexCoord2d(0,0); glVertex2f(0,200);
		//glTexCoord2d(0,1); glVertex2f(0,0);
		//glTexCoord2d(1,1); glVertex2f(200,0);
		//glTexCoord2d(1,0); glVertex2f(200,200);
		glTexCoord2d(0,0); glVertex2f(dx+0,dy+0);
		glTexCoord2d(0,cheight[ch]); glVertex2f(dx+0,dy+cheight[ch]*scale);
		glTexCoord2d(cwidth[ch],cheight[ch]); glVertex2f(dx+cwidth[ch]*scale,dy+cheight[ch]*scale);
		glTexCoord2d(cwidth[ch],0); glVertex2f(dx+cwidth[ch]*scale,dy+0);
		glEnd();
		x+=scale*cadvance[ch];
	}
}

void TFont::getMinMax(FT_GlyphSlot g, int &max_v, int &max_h)
{
	FT_Bitmap& bmp=g->bitmap;
	if (bmp.width>max_h) max_h=bmp.width;
	if (bmp.rows>max_v) max_v=bmp.rows;
}

/// @return 0 in case of error
int TFont::open(string fname)
{
	if (FT_New_Face(font_library,fname.c_str(),0,&face)) return 0;
	//FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);
	opened=1;
	initializeGlyphs();
	reloadAll();
}

void TFont::initializeGlyphs()
{
	int i,res,j,k,ch;
	int max_v, max_h;
	max_v=max_h=-1000;

	FT_Set_Char_Size( face, 32*64, 32*64, 96, 96);
	for (ch=0; ch<256; ch++)
	{
		texvalid[ch]=0;
		res=FT_Load_Glyph(face, FT_Get_Char_Index(face,ch), FT_LOAD_DEFAULT);
		if (res) continue;
		if (face->glyph->advance.x==0) continue;
		if (face->glyph->format!=FT_GLYPH_FORMAT_BITMAP)
			FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		if (face->glyph->bitmap.rows==0 && ch!=32) continue;
		getMinMax(face->glyph, max_v, max_h);
		texvalid[ch]=1;
	}
	int scale2;
	scale =FONT_SCALE_H*31/(max_v+1);
	scale2=FONT_SCALE_H*31/(max_h+1);
	if (scale2<scale) scale=scale2;

	//FT_Done_Face(face);
}

void TFont::releaseAll()
{
	if (!opened) return;
	int i;
	for (i=0; i<256; i++)
	{
		if (!textures[i]) continue;
		glDeleteTextures(1,&textures[i]);
		textures[i]=0;
	}
}

void TFont::reloadAll()
{
	if (!opened) return;
	unsigned char a[FONT_SCALE_H][FONT_SCALE_H];

	int i,res,j,k,ch;
	FT_Set_Char_Size( face, scale*64, scale*64, 96, 96);
	//DBG(FONT,"%d %d %d %d %d %d\n",min_v,max_v,min_h,max_h,max_v-min_v,max_h-min_h);
	DBG(FONT,"%d\n" ,scale);
	for (ch=0; ch<256; ch++)
	{
		if (!texvalid[ch]) continue;
		glGenTextures( 1, textures+ch);

		res=FT_Load_Glyph(face, FT_Get_Char_Index(face,ch), FT_LOAD_DEFAULT);
		if (face->glyph->format!=FT_GLYPH_FORMAT_BITMAP)
			FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
		FT_Bitmap& bmp=face->glyph->bitmap;
		for (j=0; j<FONT_SCALE_H; j++) for (k=0; k<FONT_SCALE_H; k++) a[j][k]=0;
		int disp_v=face->glyph->bitmap_top;
		int disp_h=face->glyph->bitmap_left;
		cadvance[ch] =face->glyph->advance.x / 64.0f / FONT_SCALE_H;
		cbaseline[ch]=face->glyph->bitmap_top / 1.0f / FONT_SCALE_H;
		cxdisp[ch]   =face->glyph->bitmap_left / 1.0f / FONT_SCALE_H;
		cwidth[ch]   =(face->glyph->bitmap.width+1) / 1.0f / FONT_SCALE_H;
		cheight[ch]  =(face->glyph->bitmap.rows+1) / 1.0f / FONT_SCALE_H;

		for (j=0; j<bmp.rows; j++) for (k=0; k<bmp.width; k++)
		{
			int dx=j+1;
			int dy=k+1;
			if (dx<0 || dx>=FONT_SCALE_H) continue;
			if (dy<0 || dy>=FONT_SCALE_H) continue;
			a[dx][dy]=bmp.buffer[j*bmp.width+k];
		}
		glBindTexture( GL_TEXTURE_2D, textures[ch]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,glfontaa);//GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,glfontaa);//GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		//gluBuild2DMipmaps( GL_TEXTURE_2D, GL_ALPHA8, FONT_SCALE_H, FONT_SCALE_H, GL_ALPHA, GL_UNSIGNED_BYTE, a );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA8, FONT_SCALE_H, FONT_SCALE_H, 0, GL_ALPHA, GL_UNSIGNED_BYTE, a );
		//glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE8, 64, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, a );
		texvalid[ch]=1;
		DBG(FONT,"Bitmap size char %c, W:%3d, h:%3d, v%3d, ->%d, adv:%f\n"
			,((ch<32)?'?':ch)
			,bmp.width
    		,bmp.rows ,disp_v ,disp_h
			,(face->glyph->advance.x/64.0f));
	}
}

void init_fonts()
{
	assume(!FT_Init_FreeType(&font_library),"Could not init FreeType");
}



