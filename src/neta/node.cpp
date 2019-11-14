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

// Return enum options for SymbolToken
EnumOptions<NETANode::ComparisonOperator> NETANode::comparisonOperators()
{
	static EnumOptionsList ComparisonOperatorOptions = EnumOptionsList() <<
		EnumOption(EqualTo,		"=") <<
		EnumOption(NotEqualTo,		"!=") <<
		EnumOption(GreaterThan,		">") <<
		EnumOption(LessThan,		"<") <<
		EnumOption(GreaterThanEqualTo,	">=") <<
		EnumOption(LessThanEqualTo,	"<=");
	
	static EnumOptions<NETANode::ComparisonOperator> options("ComparisonOperator", ComparisonOperatorOptions);

	return options;
}

// Constructors
NETANode::NETANode(NETADefinition* parent, NETANode::NodeType type) : ListItem<NETANode>()
{
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
 * Modifiers
 */

// Return whether the specified modifier is valid for this node
bool NETANode::isValidModifier(const char* s)
{
	return false;
}

// Set value and comparator for specified modifier
bool NETANode::setModifier(const char* modifier, ComparisonOperator op, int value)
{
	return false;
}

/*
 * Value Comparison
 */

// Return result of comparison between values provided
bool NETANode::compareValues(int lhsValue, ComparisonOperator op, int rhsValue)
{
	bool result = false;

	switch (op)
	{
		case (EqualTo):
			result = (lhsValue == rhsValue);
			break;
		case (NotEqualTo):
			result = (lhsValue != rhsValue);
			break;
		case (GreaterThan):
			result = (lhsValue > rhsValue);
			break;
		case (LessThan):
			result = (lhsValue < rhsValue);
			break;
		case (GreaterThanEqualTo):
			result = (lhsValue >= rhsValue);
			break;
		case (LessThanEqualTo):
			result = (lhsValue <= rhsValue);
			break;
		default:
			printf("Internal Error: Unrecognised operator (%i) in NETANode::valueComparison.\n", op);
			break;
	}

	return result;
}

/*
 * Scoring
 */

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
