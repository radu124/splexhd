#ifndef __HDR_player_h
#define __HDR_player_h


/**
 * return level state of the given level
 * @param level the number of the level in the current level set
 */
int levelstate(int level);

/**
 * end the current level, either with success or failure
 * @param level
 * @param victory
 * @param time
 */
void finishlevel(int level, int victory, int time);

/**
 * load the player statistics from the scores file
 */
void loadplayerstate();


#endif
