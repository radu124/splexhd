#ifndef __HDR_objKnik_h
#define __HDR_objKnik_h


/** @file objKnik.cpx functions specific to scissors and electrons */

void doobj_knik(int loc);
void display_knik(int loc);
void display_electron(int loc);

/**
 * Initialize knick-knack or electrons at the start of the level
 * the procedure is the following:
 *  - by default turn left anticlockwise
 *  - if left is free skip one turn, done
 *  - if above is free move above rightaway, done
 *  - if right is free move right rightaway
 */
void initobj_knik(int loc);


#endif
