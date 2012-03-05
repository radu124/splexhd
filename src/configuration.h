#ifndef __HDR_configuration_h
#define __HDR_configuration_h


#include "includes.h"
#include "confrw.h"

extern string configFile;
extern string scoresFile;
extern string homeconfdir;
extern string datadir;

#define CONFIGLIST \
CONFIGITEM(int,video_glflush,0) \
CONFIGITEM(int,video_glfinish,0) \
CONFIGITEM(int,video_fpslimit,0) \
CONFIGITEM(int,video_dofullscreen,0) \
CONFIGITEM(int,video_overrideres,0) \
CONFIGITEM(int,scr_width,800) \
CONFIGITEM(int,scr_height,600) \


#define CI_DECLARE(a,b,c) a b=c;
#define CI_DECLARE_EXTERN(a,b,c) extern a b;
#define CI_PRINT(a,b,c) { fprintf(fou,""#b" = "); CONFWRITE_##a(b); fprintf(fou,"\n"); }
#define CIA_PRINT(a,b,c,d) { int it; for (it=0; it<b.size(); it++) { fprintf(fou,""#b"[%d]"#d" = ",it); CONFWRITE_##a(b[it]c); fprintf(fou,"\n");} }

#define CI_READ(a,b,c) { if (tsimatch(p,""#b)) { confread_##a(p,b); } }
#define CIA_READ(a,b,c,d) { int idx; if (tsamatch(p,""#b,""#d,idx)) \
	{\
		assume(idx<1000,"Bad array index>1000"); \
		if (idx>=b.size()) b.resize(idx+1); \
		confread_##a(p,b[idx]c); \
	}\
}

/*******************************************************
 * Automatic generation of config variable declarations
 */
#define CONFIGITEM CI_DECLARE_EXTERN
#define CONFIGARRAY(a,b,c,d)
CONFIGLIST
#undef CONFIGARRAY
#undef CONFIGITEM


// bit mask describing the last selected input devices
//we don't expect more than 8 players
void init_config();
void config_write();

extern vector<string> config_unknown;

/***********************************************
 * Match configuration string against pattern
 * and increment pointer if match occurred
 */
int tsimatch(char *&s, const char *v);
int tsamatch(char *&s, const char *prefix, const char *suffix, int &idx);


#endif
