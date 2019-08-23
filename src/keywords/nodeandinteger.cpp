/*
	*** Keyword - Node and Integer
	*** src/keywords/nodeandinteger.cpp
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

#include "keywords/nodeandinteger.h"
#include "procedure/nodes/node.h"

// Constructor
NodeAndIntegerKeywordBase::NodeAndIntegerKeywordBase(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType)
{
	parentNode_ = parentNode;
	nodeType_ = nodeType;
}

// Destructor
NodeAndIntegerKeywordBase::~NodeAndIntegerKeywordBase()
{
}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode* NodeAndIntegerKeywordBase::parentNode() const
{
	return parentNode_;
}

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeAndIntegerKeywordBase::nodeType() const
{
	return nodeType_;
}

