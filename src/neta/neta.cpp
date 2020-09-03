/*
    *** NETA Definition
    *** src/neta/neta.cpp
    Copyright T. Youngs 2015-2020

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

#include "neta/neta.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "data/ffatomtype.h"
#include "neta/generator.h"
#include "neta/node.h"
#include <stdarg.h>
#include <string.h>

NETADefinition::NETADefinition() : rootNode_(this) {}

NETADefinition::~NETADefinition() { rootNode_.clear(); }

/*
 * Data
 */

// Return root node pointer
NETARootNode *NETADefinition::rootNode() { return &rootNode_; }

// Create definition from stored string
bool NETADefinition::create(const Forcefield *associatedFF)
{
    rootNode_.clear();

    return NETADefinitionGenerator::generate(*this, definitionString_, associatedFF);
}

// Set generating string
void NETADefinition::setDefinitionString(std::string_view definition) { definitionString_ = definition; }

// Return original generating string
std::string_view NETADefinition::definitionString() const { return definitionString_; }

/*
 * Matching
 */

// Check supplied atom to see if it matches this NETA description
int NETADefinition::score(const SpeciesAtom *i) const
{
    RefList<const SpeciesAtom> matchPath;
    return rootNode_.score(i, matchPath);
}
