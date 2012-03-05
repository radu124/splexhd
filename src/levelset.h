#ifndef __HDR_levelset_h
#define __HDR_levelset_h


#include "includes.h"

/**
 * load a set of levels into memory
 * @param filename - the name of the file, this can be either an index file
 * or a .DAT file containing all the levels (ideally, !! TODO !!)
 *
 * an index file contains a list of files, one per line
 * each file representing a separate level
 */
int load_levelset(string filename);

/**
 * get a pointer to the data of one of the levels
 */
const uint8_t* getleveldata(int levelnum);

/**
 * get the name of a level
 */
string getlevelname(int levelnum);

/**
 * get the hash of the level
 */
string getlevelhash(int levelnum);

/**
 * get the number of levels in the currently loaded levelset
 */
int getnumlevels();



#endif
