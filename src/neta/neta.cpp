/*
	*** NETA Definition
	*** src/neta/neta.cpp
	Copyright T. Youngs 2015-2019

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
#include "neta/generator.h"
#include "neta/node.h"
#include "data/ffatomtype.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <stdarg.h>
#include <string.h>

// Constructor
NETADefinition::NETADefinition(const char* netaDefinition, const Forcefield* associatedFF) : rootNode_(this, NETANode::BasicNode)
{
	set(netaDefinition, associatedFF);
}

// Destructor
NETADefinition::~NETADefinition()
{
	clear();
}

/*
 * Data
 */

// Clear contents of definition
void NETADefinition::clear()
{
	definitionString_.clear();
	rootNode_.clear();
}

// Return root node pointer
NETANode* NETADefinition::rootNode()
{
	return &rootNode_;
}

// Set NETADefinition from supplied string
bool NETADefinition::set(const char* netaDefinition, const Forcefield* associatedFF)
{
	clear();

	definitionString_ = netaDefinition;

	bool result = NETADefinitionGenerator::generate(*this, definitionString_.get(), associatedFF);
	if (!result) Messenger::error("Failed to generate NETA definition from input string '%s'.\n", netaDefinition);

	return result;
}

// Set generating string
void NETADefinition::setDefinitionString(const char* definition)
{
	definitionString_ = definition;
}

// Return original generating string
const char* NETADefinition::definitionString() const
{
	return definitionString_.get();
}

/*
 * Matching
 */

// Check supplied atom to see if it matches this NETA description
int NETADefinition::score(const SpeciesAtom* i) const
{
	// Our entry point is the branch of our BasicNode - if there are no branch nodes, return zero (== match anything)
	if (rootNode_.nBranchNodes() == 0) return 0;

	RefList<const SpeciesAtom> matchPath;
	int totalScore = rootNode_.score(i, matchPath);

	return totalScore;
}
