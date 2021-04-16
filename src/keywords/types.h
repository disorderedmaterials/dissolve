// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
#include "keywords/expressionvariablevector.h"
#include "keywords/fileandformat.h"
#include "keywords/geometrylist.h"
#include "keywords/isotopologueset.h"
#include "keywords/linkto.h"
#include "keywords/module.h"
#include "keywords/modulegroups.h"
#include "keywords/modulevector.h"
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
#include "keywords/vector_stringpair.h"
