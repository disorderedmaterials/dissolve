/*
	*** Simple Pointer Array Class
	*** src/templates/pointerarray.h
	Copyright T. Youngs 2012-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_POINTERARRAY_H
#define DISSOLVE_POINTERARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "base/messenger.h"

// Forward Declarations
/* none */

/*
 * Simple Pointer Array Class
 * A contiguous pointer array class designed to be as lightweight as possible.
 * Order of items must not be important, as the arrangement of pointers is subject to change.
 */

template <class T> using PointerArray = std::vector<T*>;

#endif
