#ifndef __HDR_includes_h
#define __HDR_includes_h


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "assert.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ft2build.h>
#include <png.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include <vorbis/vorbisfile.h>

#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;
typedef vector<string> vstring;
typedef vector<vstring> vvstring;
typedef vector<int> vint;
typedef vector<vint> vvint;

#ifdef _WINDOWS
#include <windows.h>
#include <shlobj.h>
#endif

struct mcolor{
	GLfloat Red,Green,Blue,Alpha;
	void set(); // in utils.cpx
	void weight(mcolor a, mcolor b, GLfloat wg);
	mcolor():Red(0),Green(0),Blue(0),Alpha(0) {;}
	mcolor(GLfloat r, GLfloat g, GLfloat b):Red(r),Green(g),Blue(b),Alpha(0) {;}
	mcolor(GLfloat r, GLfloat g, GLfloat b, GLfloat a):Red(r),Green(g),Blue(b),Alpha(a) {;}
};


#endif
