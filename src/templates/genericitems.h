/*
	*** Generic Items
	*** src/templates/genericitems.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_GENERICITEMS_H
#define DUQ_GENERICITEMS_H

/*
 * This file should be included if the use of GenericItems is required.
 * It simply includes all other GenericItemContainer specialisations,
 * followed by the templated GenericItemContainer.
 */

#include "templates/genericitemcontainer_bool.h"
#include "templates/genericitemcontainer_int.h"
#include "templates/genericitemcontainer_double.h"
#include "templates/genericitemcontainer_charstring.h"

#include "templates/genericitemcontainer_vec3int.h"
#include "templates/genericitemcontainer_vec3double.h"

#include "templates/genericitemcontainer_array2ddouble.h"
#include "templates/genericitemcontainer_array.h"
#include "templates/genericitemcontainer_arraydouble.h"
#include "templates/genericitemcontainer_arrayint.h"
#include "templates/genericitemcontainer_arrayvec3int.h"
#include "templates/genericitemcontainer_arrayvec3double.h"

#include "templates/genericitemcontainer.h"

#endif
