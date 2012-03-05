#ifndef __HDR_gamedata_h
#define __HDR_gamedata_h

#include "includes.h"

/**
 * Object heading constants, define the direction objects move in
 */
enum TTHEADING {
	TH_UP=0,
	TH_RIGHT=1,
	TH_DOWN=2,
	TH_LEFT=3,
	TH_MOVEIN=4,
	TH_ANTICLOCK=0x08,
	TH_CLOCKWISE=0x0c
};
/**
 * Additional heading constants due to value overlap
 */
enum TTHEADINGM {
	TH_NORMAL=0x00,      // object is in a normal state, not going anywhere
	TH_VANISHING=0x01,   // adjacent objects eaten by murphy without moving
	TH_PREPAREFALL=0x02, // zonks and infotrons have a special state, prepare to fall
	TH_MOVEDOUBLE=0x08   // murphy can move two squares through a port
};

/**
 * vanishing object types
 */
enum TTVANISHING {
	TV_REDDISK=0x05,
	TV_EXPLODE=0x06,
	TV_EXPLODEINFO=0x07
};

/**
 * Tile type constants, defined as enum for simplicity
 */
enum TTFLAGS {
	// TB = tile base - this is the same id the tile has in the level file
	TB_ZONK        =    0x01,
	TB_PCB         =    0x02,
	TB_MURPHY      =    0x03,
	TB_INFOTRON    =    0x04,
	TB_CONCRETE    =    0x06,
	TB_EXIT        =    0x07,
	TB_ORANGE      =    0x08,
	TB_PORTRIGHT   =    0x09,
	TB_PORTDOWN    =    0x0a,
	TB_PORTLEFT    =    0x0b,
	TB_PORTUP      =    0x0c,
	TB_KNIK        =    0x11,
	TB_YELLOW      =    0x12,
	TB_CONSOLE     =    0x13,
	TB_REDDISK     =    0x14,
	TB_PORTUPDN    =    0x15,
	TB_PORTLR      =    0x16,
	TB_PORTALL     =    0x17,
	TB_ELECTRON    =    0x18,
	TB_BUG         =    0x19,
	TB_LEVTITLE    =    0x3f,
	TB_MYTILE      =    0x29, // 41
	// TX = extended IDs we assign here to tiles that move
	TX_VANISHING   =    0x70,
	TX_KNIK        =    0x80,
	TX_ELECTRON    =    0x90,
	TX_ORANGE      =    0xa0,
	TX_YELLOW      =    0xb0,
	TX_ZONK        =    0xc0,
	TX_INFOTRON    =    0xd0,
	TX_MURPH       =    0xe0,
	TX_MOVEOUT     =    0xf0,
	// TF = tile flags, put tiles in categories to facilitate decisions
	// TD = flag combinations, to avoid checking for flags with plain &
	TF_BUG         =  0x0100,
	TF_IC          =  0x0200,
	TF_SOLID       =  0x0400,
	TF_PORT        =  0x0800,
	TF_SPECIAL     =  0x1000,
	TD_PORTSPECIAL =  0x1800
};

/**
 * one map tile
 * typ - indicates the tile type
 * counter - indicates the progress of the current tile animation
 * xplode - indicates the time until an explosion will take place in that tile
 */
struct ttile
{
	int typ;     // aka extd
	int counter; // aka basic, cdown
	int xplode;
	int oldtyp;
	void set(int vtyp=0, int vcounter=0, int vexplode=0);
	inline int basetyp() { return typ&0xff; } // base type
	inline int xtyp() { return typ&0xf0; } // base type
	inline int movtyp() { return typ & 0x0c; }
	inline int movfull() { return typ & 0x0f; }
	inline int movdirection() { return typ & 0x03; }
	inline int isfree() { return typ==0; }
	inline int isfreeormurph() { return typ==0 || xtyp()==TX_MURPH; }
	inline int isic() { return (typ & TF_IC)!=0; }
	inline int ispcb() { return basetyp()==TB_PCB; }
	inline int isenemy() { return xtyp()==TX_KNIK || xtyp()==TX_ELECTRON; }
	inline int isportup()    { int tb=basetyp(); return tb==TB_PORTUP    || tb==TB_PORTUPDN || tb==TB_PORTALL; }
	inline int isportdown()  { int tb=basetyp(); return tb==TB_PORTDOWN  || tb==TB_PORTUPDN || tb==TB_PORTALL; }
	inline int isportleft()  { int tb=basetyp(); return tb==TB_PORTLEFT  || tb==TB_PORTLR   || tb==TB_PORTALL; }
	inline int isportright() { int tb=basetyp(); return tb==TB_PORTRIGHT || tb==TB_PORTLR   || tb==TB_PORTALL; }
	int isdeadly(); // defined in objMurph
/*
#define ISFREEFORZONK(coord) (ISFREE(coord) || ((base==0x10) && (ldt[coord].base==0x0200)))
#define ISLATRSVD(coord) (ldt[coord].b.extd==0xfd)
#define ISREDDISK(coord) (ldt[coord].b.extd==0xe2)
#define ISDETONATOR(coord) (ldt[coord].b.extd==0xe5)
#define ISEXIT(coord) (ldt[coord].b.extd==0xe4)
#define ISTKTRON(coord) (ldt[coord].b.extd==0x2f || ldt[coord].b.extd==0x2e)
#define ISFREEORMURPH(coord) (ISFREE(coord) || ISMURPHVULNERABLE(coord))
#define ISMURPHVULNERABLE(coord) (ldt[coord].b.extd>=0x80 && ldt[coord].b.extd<0xa0)
*/

};

/**
 * large table containing the entire map data and current status
 * initialized by loadLevel(), never deallocated, only increased in size
 *    to accommodate larger levels
 */
extern ttile *ldt;

/**
 * amount of memory allocated for ldt
 */
extern int ldt_allocd;

/**
 * memory for gameplay demo
 * a length of 0 indicates no demo available
 */
struct tdemo
{
	int length;
	int pos;
	int move;
	int mcnt;
	int frame;
	uint8_t dat[16384];
	void init();
};

enum enum_gamemode{
	GAMEMODE_NORMAL,
	GAMEMODE_LOOKAROUND,
	GAMEMODE_MENU
	};

/**
 * just to group together the variables
 * comprising the game state
 */
struct tgamestate
{
	int keydown[8];       /// keys currently being pressed: UP,RIGHT,DOWN,LEFT,SPACE
	int mode; //=0;
	int victory;          /// 1 if victory
	int ended;            /// whether the game ended, either win or lose
	int loadlevel;        /// whether a new level has to be loaded
	int lastlevel;        /// the last level that was loaded

	int gravity;          /// whether gravity is on
	int freezezonks;
	int freezeenemies;
	int infotronsneeded;  /// infotrons needed to finish the level
	int reddiskcount;     /// number of red disks available
	int reddisklocation;  /// location of dropped red disk
	int reddisktimeout;   /// timeout of murphy laying a red disk

	int murphloc;         /// murphy's location
	int murphlastleft;    /// whether murphy's last move was to the left
	int murphsdinner;     /// type of tile currently being eaten
	int murphlookat;      /// direction murphy is looking into after grab
	int forcecenter;      /// center on murphy at any cost

	int timestarted;      /// the time the level was started in milliseconds
	int timepaused;       /// the time the level was paused n/a
	int timenow;          /// time at current level updated
	int timelastmove;     /// time of the last step

	int sfxzonkleft;      /// for producing sound effects on zonk fall, stereo
	int sfxzonkright;
	int sfxexplodeleft;   /// for producing explosion sound effects, stereo
	int sfxexploderight;

	void init();
};

extern tgamestate game;
extern string levelname;
extern tdemo demo;

extern int lvlwidth;  /// width of a level
extern int lvlheight; /// height of a level

inline int above(int i) { int res=i-lvlwidth; return res<0?i:res; }
inline int below(int i) { int res=i+lvlwidth; return res>=(lvlwidth*lvlheight)?i:res; }
inline int leftof(int i) { int res=i-1; return res<0?i:res; }
inline int rightof(int i) { int res=i+1; return res>=(lvlwidth*lvlheight)?i:res; }



#endif
