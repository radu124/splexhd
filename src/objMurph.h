#ifndef __HDR_objMurph_h
#define __HDR_objMurph_h


/**
 * take care of Murphy's movement
 * @param loc Murphy's location (the tile he is entering)
 *
 * movement types are:
 * TH_VANISHING - Murphy got to an exit
 * TH_MOVEIN - normal move in
 * TH_MOVEDOUBLE - move through a port
 */
void doobj_murphy(int loc);

/**
 * display Murphy moving
 * @param loc Murphy's location (the tile he is entering)
 * TH_MOVEIN will also print game.murphsdinner
 */
void display_murphy(int loc);

void zoomon_murphy(int loc);


#endif
