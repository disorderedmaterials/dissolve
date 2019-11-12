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
#include "templates/reflist.h"
#include <stdarg.h>
#include <string.h>
#include <cctype>

// Local constant
const int MAXNODEARGS = 10;

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
 * Scoring
 */

// Set node to use reverse logic
void NETANode::setReverseLogic()
{
	reverseLogic_ = true;
}
