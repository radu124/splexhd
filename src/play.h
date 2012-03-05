#ifndef __HDR_play_h
#define __HDR_play_h


#include "includes.h"

extern int lvlshowdispi;
extern int lvlshowdispj;
extern int lastframetime;
extern int thisframetime;
extern int tcnti,tcntj;
extern int relyonrefresh;
extern int slomo;
extern int slocount;
extern int expectedtimeperframe;


extern uint32_t timenow;
extern uint32_t timestarted;
extern uint32_t showlevstat;

extern uint64_t timerfreq;

void startgame();
void playGame();
void initLevel();
void repairBorder();
void countDown();
void doObjects();



/**
 * returns the progress of an object through animation
 * @param counter    - the object counter
 * @param maxcounter - the maximum value of the counter
 * @param animstart - value at the start of the animation
 * @param animstop  - the value at the end of animation, for example if animstart is 0 and animstop is 100 then the result in in percent
 */
int animprogress(int counter, int maxcounter, int animstart, int animstop=0);


#endif
