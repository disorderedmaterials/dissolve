/*
	*** Node Scope Stack
	*** src/procedure/nodescope.cpp
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

#include "procedure/nodescope.h"
#include "base/sysfunc.h"

// Constructor
NodeScope::NodeScope(ProcedureNode::NodeContext context) : ListItem<NodeScope>()
{
	context_ = context;
}

// Destructor
NodeScope::~NodeScope()
{
}

/*
 * Node Context / Content
 */

// Return the current context
ProcedureNode::NodeContext NodeScope::context() const
{
	return context_;
}

// Add new node to the topmost scope layer
bool NodeScope::add(ProcedureNode* node)
{
	if (!node) return false;

	// Before we add the node, check that our context allows it
	if (!node->isContextRelevant(context_)) return Messenger::error("Node type '%s' is not allowed in the '%s' context.\n", ProcedureNode::nodeTypes().keyword(node->type()), ProcedureNode::nodeContexts().keyword(context_));

	nodes_.add(node);

	return true;
}

/*
 * Node Search
 */

// Return named node if known, and which matches the (optional) type given
ProcedureNode* NodeScope::node(const char* name, ProcedureNode::NodeType nt) const
{
	RefListIterator<ProcedureNode,bool> nodeIterator(nodes_);
	while (ProcedureNode* node = nodeIterator.iterate())
	{
		if (DissolveSys::sameString(node->name(), name))
		{
			// Check type
			if (nt == ProcedureNode::nNodeTypes) return node;
			else if (node->type() == nt) return node;
		}
	}

	return NULL;
}
