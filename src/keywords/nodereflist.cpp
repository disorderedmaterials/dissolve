/*
	*** Keyword - Node RefList
	*** src/keywords/nodereflist.cpp
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

#include "keywords/nodereflist.h"
#include "procedure/nodes/node.h"

// Constructor
NodeRefListKeywordBase::NodeRefListKeywordBase(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope)
{
	parentNode_ = parentNode;
	nodeType_ = nodeType;
	onlyInScope_ = onlyInScope;
}

// Destructor
NodeRefListKeywordBase::~NodeRefListKeywordBase()
{
}

/*
 * Parent Node
 */

// Parent ProcedureNode
ProcedureNode* NodeRefListKeywordBase::parentNode() const
{
	return parentNode_;
}

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeRefListKeywordBase::nodeType() const
{
	return nodeType_;
}

// Return whether to accept nodes within scope only
bool NodeRefListKeywordBase::onlyInScope() const
{
	return onlyInScope_;
}
