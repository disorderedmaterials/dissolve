/*
	*** General Node for NETA
	*** src/expression/node.cpp
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

#include "neta/node.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "neta/connection.h"
#include "templates/reflist.h"
#include <stdarg.h>
#include <string.h>
#include <cctype>

// Constructors
NETANode::NETANode(NETADefinition* parent, NETANode::NodeType type) : ListItem<NETANode>()
{
	repeatCount_ = 1;
	repeatCountOperator_ = NETANode::EqualTo;
	reverseLogic_ = false;
	parent_ = parent;
	nodeType_ = type;
}

// Destructor
NETANode::~NETANode()
{
	clear();
}

/*
 * Node Type and Parent
 */

// Return node type
NETANode::NodeType NETANode::nodeType() const
{
	return nodeType_;
}

// Return parent
NETADefinition* NETANode::parent() const
{
	return parent_;
}

/*
 * Branching and Node Generation
 */

// Clear all nodes
void NETANode::clear()
{
	branch_.clear();
}

// Return last node of branch
NETANode* NETANode::lastBranchNode()
{
	return branch_.last();
}

// Return number of nodes defined in branch
int NETANode::nBranchNodes() const
{
	return branch_.nItems();
}


// Create connectivity node from current targets
NETAConnectionNode* NETANode::createConnectionNode(PointerArray<Element> targetElements, PointerArray<ForcefieldAtomType> targetAtomTypes)
{
	// Create the new node and own it
	NETAConnectionNode* node = new NETAConnectionNode(parent_, targetElements, targetAtomTypes);
	branch_.own(node);

	return node;
}

/*
 * Scoring
 */

// Set repeat count value and comparison operator
void NETANode::setRepeatCount(int value, NETANode::ComparisonOperator op)
{
	repeatCount_ = value;
	repeatCountOperator_ = op;
}

// Set node to use reverse logic
void NETANode::setReverseLogic()
{
	reverseLogic_ = true;
}

// Evaluate the node and return its score
int NETANode::score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const
{
	int totalScore = 0;

	// Loop over nodes in branch in sequence
	ListIterator<NETANode> branchIterator(branch_);
	while (NETANode* node = branchIterator.iterate())
	{
		// Get the score from the node, returning early if NoMatch is encountered
		int nodeScore = node->score(i, matchPath);
		if (nodeScore == NETANode::NoMatch) return NETANode::NoMatch;

		totalScore += nodeScore;
	}

	return totalScore;
}
