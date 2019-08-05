/*
	*** Keywords
	*** src/module/keywords.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_KEYWORDTYPES_H
#define DISSOLVE_KEYWORDTYPES_H

/*
 * This file should be included if the use of Keywords is required.
 * It simply includes all other derived classes and specialisations.
 */

// PODs
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/integer.h"

// Classes
#include "keywords/atomtypeselection.h"
#include "keywords/broadeningfunction.h"
#include "keywords/charstring.h"
#include "keywords/complex.h"
#include "keywords/data1dstore.h"
#include "keywords/data2dstore.h"
#include "keywords/data3dstore.h"
#include "keywords/enumoptions.h"
#include "keywords/fileandformat.h"
#include "keywords/isotopologuelist.h"
#include "keywords/modulegroups.h"
#include "keywords/modulereferencelist.h"
#include "keywords/pairbroadeningfunction.h"
#include "keywords/procedure.h"
#include "keywords/speciesreferencelist.h"
#include "keywords/speciessite.h"
#include "keywords/speciessitereferencelist.h"
#include "keywords/vec3double.h"
#include "keywords/vec3integer.h"
#include "keywords/windowfunction.h"

#endif
