/*******************************************************************
(C) 2011 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/

#include "includes.h"

void mcolor::weight(mcolor a, mcolor b, GLfloat wg)
{
	Red   = a.Red   *(1-wg) + b.Red   * wg;
	Green = a.Green *(1-wg) + b.Green * wg;
	Blue  = a.Blue  *(1-wg) + b.Blue  * wg;
	Alpha = a.Alpha *(1-wg) + b.Alpha * wg;
}



