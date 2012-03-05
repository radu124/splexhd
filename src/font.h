#ifndef __HDR_font_h
#define __HDR_font_h


#include "includes.h"
#include "utils.h"

class TFont
{
public:
	static const int FONT_SCALE_H=64;
	static const int NUM_CHARS=256;
	enum {ALIGN_LEFT=0, ALIGN_CENTER=1, ALIGN_RIGHT=2, ALIGN_VONETHIRD=4, ALIGN_VHALFA=8};
	int scale;
	int opened;
	int open(string fname);
	GLuint textures[NUM_CHARS];
	int texvalid[NUM_CHARS];
	GLfloat cadvance[NUM_CHARS];
	GLfloat cwidth[NUM_CHARS];
	GLfloat cheight[NUM_CHARS];
	GLfloat cbaseline[NUM_CHARS];
	GLfloat cxdisp[NUM_CHARS];

	FT_Face face;
	TFont():opened(0){;}
	void getMinMax(FT_GlyphSlot g, int &max_v, int &max_h);
	void displayString(const char *str,GLfloat x, GLfloat y, GLfloat scale, int pos=0);
	void initializeGlyphs();
	void releaseAll();
	void reloadAll();
};

void init_fonts();

/**
 * default font
 */
extern TFont deffont;


#endif
