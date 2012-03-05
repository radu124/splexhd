#ifndef __HDR_loadlevel_h
#define __HDR_loadlevel_h


#include "includes.h"

/**
 * load and initialize a game level into ldt
 * also initialize game and demo structures
 * @param leveldata pointer to level data
 * @return 1 on success, 0 otherwise
 */
int loadLevel(const uint8_t* leveldata);


/**
 * set the borders around the level to solid blocks
 */
void repairBorder();


#endif
