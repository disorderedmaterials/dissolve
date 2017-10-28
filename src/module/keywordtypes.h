/*
	*** Module Keyword - Integer
	*** src/modules/modulekeyword_int.h
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

#ifndef DUQ_MODULEKEYWORDS_H
#define DUQ_MODULEKEYWORDS_H

/*
 * This file should be included if the use of ModuleKeywords is required.
 * It simply includes all other derived classes and specialisations.
 */

// PODs
#include "modules/modulekeyword_bool.h"
#include "modules/modulekeyword_double.h"
#include "modules/modulekeyword_int.h"

// Complex
#include "modules/modulekeyword_complex.h"

// Classes
#include "modules/modulekeyword_charstring.h"
#include "modules/modulekeyword_broadeningfunction.h"

#endif
