/*
	*** Keyword - Node Array
	*** src/keywords/nodearray.cpp
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

#include "keywords/nodearray.h"
#include "procedure/nodes/node.h"

// Constructors
NodeArrayKeywordBase::NodeArrayKeywordBase(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope, bool uniqueNodes)
{
	parentNode_ = parentNode;
	nodeType_ = nodeType;
	onlyInScope_ = onlyInScope;
	isVariableSize_ = true;
	fixedArraySize_ = -1;
	nodesAreUnique_ = uniqueNodes;
}
NodeArrayKeywordBase::NodeArrayKeywordBase(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType, int fixedArraySize, bool onlyInScope, bool uniqueNodes)
{
	parentNode_ = parentNode;
	nodeType_ = nodeType;
	onlyInScope_ = onlyInScope;
	isVariableSize_ = false;
	fixedArraySize_ = fixedArraySize;
	nodesAreUnique_ = uniqueNodes;
}

// Destructor
NodeArrayKeywordBase::~NodeArrayKeywordBase()
{
}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode* NodeArrayKeywordBase::parentNode() const
{
	return parentNode_;
}

/*
 * Node Type and Array Size
 */

// Return target node type to allow
ProcedureNode::NodeType NodeArrayKeywordBase::nodeType() const
{
	return nodeType_;
}

// Return whether to accept nodes within scope only
bool NodeArrayKeywordBase::onlyInScope() const
{
	return onlyInScope_;
}

// Return whether this is a variable size array
bool NodeArrayKeywordBase::isVariableSize() const
{
	return isVariableSize_;
}

// Return size of fixed array (if relevant)
int NodeArrayKeywordBase::fixedArraySize() const
{
	return fixedArraySize_;
}

// Return whether nodes in the array must all be unique
bool NodeArrayKeywordBase::nodesAreUnique() const
{
	return nodesAreUnique_;
}
