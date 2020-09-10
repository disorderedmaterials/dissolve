/*
    *** Keyword Types
    *** src/keywords/types.h
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
 * This file should be included if the use of Keywords is required.
 * It simply includes all other derived classes and specialisations.
 */

// PODs
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/integer.h"

// Classes
#include "keywords/atomtypereflist.h"
#include "keywords/atomtypeselection.h"
#include "keywords/broadeningfunction.h"
#include "keywords/data1dstore.h"
#include "keywords/data2dstore.h"
#include "keywords/data3dstore.h"
#include "keywords/dynamicsitenodes.h"
#include "keywords/elementreflist.h"
#include "keywords/enumoptions.h"
#include "keywords/expression.h"
#include "keywords/expressionvariablelist.h"
#include "keywords/fileandformat.h"
#include "keywords/geometrylist.h"
#include "keywords/isotopologuecollection.h"
#include "keywords/linkto.h"
#include "keywords/module.h"
#include "keywords/modulegroups.h"
#include "keywords/modulereflist.h"
#include "keywords/node.h"
#include "keywords/nodeandinteger.h"
#include "keywords/nodearray.h"
#include "keywords/nodebranch.h"
#include "keywords/nodereflist.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevalueenumoptions.h"
#include "keywords/pairbroadeningfunction.h"
#include "keywords/procedure.h"
#include "keywords/range.h"
#include "keywords/species.h"
#include "keywords/speciesreflist.h"
#include "keywords/speciessite.h"
#include "keywords/speciessitereflist.h"
#include "keywords/stdstring.h"
#include "keywords/vec3double.h"
#include "keywords/vec3integer.h"
#include "keywords/vec3nodevalue.h"
#include "keywords/vector_intdouble.h"
#include "keywords/vector_intstring.h"
#include "keywords/windowfunction.h"
