#ifndef __HDR_display_h
#define __HDR_display_h

#include "includes.h"

void renderscene();

/**
 * Compute exact object location on screen, including modifier
 * @param loc - tile index
 * @param locy - position on the y (vertical) axis, axis points down
 * @param locx - position on the x (horizontal) axis, axis points right
 */
void computeloc(int loc, GLfloat &locy, GLfloat &locx, int flags=0);

/**
 * paint a texture at the given coordinates
 * @param i horizontal location
 * @param j vertical location
 * @param id texture id inside a group of textures
 * @param group the group of textures
 * @param rot rotation in degrees
 */
void painttex(GLfloat i, GLfloat j, int id, int group=0, int rot=0, int flags=0);


#endif
