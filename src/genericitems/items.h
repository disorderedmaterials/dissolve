/*
    *** Generic Items
    *** src/genericitems/items.h
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

#pragma once

/*
 * This file should be included if the use of GenericItems is required.
 * It simply includes all other GenericItemContainer specialisations,
 * followed by the templated GenericItemContainer.
 */

#include "genericitems/bool.h"
#include "genericitems/double.h"
#include "genericitems/int.h"
#include "genericitems/stdstring.h"
#include "genericitems/streampos.h"

#include "genericitems/vec3double.h"
#include "genericitems/vec3int.h"

#include "genericitems/array.h"
#include "genericitems/array2d.h"
#include "genericitems/array2darraydouble.h"
#include "genericitems/array2dbool.h"
#include "genericitems/array2ddouble.h"
#include "genericitems/array2ddummy.h"
#include "genericitems/arraydouble.h"
#include "genericitems/arraydummy.h"
#include "genericitems/arrayint.h"
#include "genericitems/arrayvec3double.h"
#include "genericitems/arrayvec3int.h"

#include "genericitems/container.h"
