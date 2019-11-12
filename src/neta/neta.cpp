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
NETADefinition::NETADefinition(const char* netaDefinition, const Forcefield* associatedFF)
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

// Clear contents of expression
void NETADefinition::clear()
{
	nodes_.clear();
	rootNode_ = NULL;
}

// Set NETADefinition from supplied string
bool NETADefinition::set(const char* netaDefinition, const Forcefield* associatedFF)
{
	clear();

	definitionString_ = netaDefinition;

	return (definitionString_.isEmpty() ? false : NETADefinitionGenerator::generate(*this, definitionString_.get(), associatedFF));
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
 * Nodes
 */

// Set the root node representing the definition
void NETADefinition::setRootNode(NETANode* node)
{
	if (rootNode_) Messenger::error("A root node already exists for this NETA definition - it is about to be replaced...\n");

	rootNode_ = node;
}

// Join two nodes together with the specified logic to form a sequence
NETALogicNode* NETADefinition::joinWithLogic(NETANode* node1, NETALogicNode::LogicType logic, NETANode* node2)
{
	NETALogicNode* newNode = new NETALogicNode(this, logic, node1, node2);

	nodes_.own(newNode);

	return newNode;
}

// Create connectivity node from current targets
NETAConnectionNode* NETADefinition::createConnectionNode(PointerArray<Element> targetElements, PointerArray<ForcefieldAtomType> targetAtomTypes, SpeciesBond::BondType bt, NETANode* innerDefinition)
{
	// Create the new node and own it
	NETAConnectionNode* node = new NETAConnectionNode(this, targetElements, targetAtomTypes, bt, innerDefinition);
	nodes_.own(node);

	return node;
}

/*
 * Matching
 */

// Check supplied atom to see if it matches this NETA description
int NETADefinition::score(const SpeciesAtom* i) const
{
	// If there is no definition, return zero (== match anything)
	if (!rootNode_) return NETANode::NoMatch;

	RefList<const SpeciesAtom> matchPath;
	int totalScore = rootNode_->score(i, matchPath);

	return totalScore;
}
