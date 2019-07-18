/*
	*** Node Scope Stack
	*** src/procedure/nodescopestack.cpp
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

#include "procedure/nodescopestack.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/select.h"
#include "base/sysfunc.h"

// Constructor
NodeScopeStack::NodeScopeStack()
{
	nSelectNodesAdded_ = 0;
}

// Destructor
NodeScopeStack::~NodeScopeStack()
{
}

/*
 * Stack
 */

// Clear all layers from stack
void NodeScopeStack::clear()
{
	stack_.clear();
	nodes_.clear();

	nSelectNodesAdded_ = 0;
}

// Push new scope on to the stack
void NodeScopeStack::push()
{
	stack_.add(RefList<ProcedureNode,bool>());
}

// Pop topmost scope from the stack
bool NodeScopeStack::pop()
{
	if (stack_.nItems() == 0) return Messenger::error("Scope stack is empty, so nothing to pop().\n");

	stack_.removeLast();

	return true;
}

// Add new node to the topmost scope layer
bool NodeScopeStack::add(ProcedureNode* node)
{
	// Check that we have a scope to add to
	if (stack_.nItems() == 0)
	{
		Messenger::error("No current scope in which to add node '%s'.\n", node->name());
		return false;
	}

	// Check that the name is valid
	if (nodeInScope(node->name()))
	{
		Messenger::error("A node with name '%s' is already in scope.\n", node->name());
		return false;
	}

	// Increase the general counter for new references, and add it
	++nSelectNodesAdded_;

	stack_.last().add(node);
	nodes_.add(node);

	return true;
}

/*
 * Node Search
 */

// Return next available generic name for a SelectNode
const char* NodeScopeStack::nextSelectName() const
{
	// Generic names range from A-Z, then AA through to AZ etc. (should we ever need that many)
	static CharString result;
	int nAZ = nSelectNodesAdded_ / 26;
	if (nAZ == 0) result.sprintf("%c", 65+nSelectNodesAdded_);
	else result.sprintf("%c%c", 64+nAZ, 65+(nSelectNodesAdded_%26));

	return result.get();
}

// Return named node if it is currently in scope, and optionally matches the type given
ProcedureNode* NodeScopeStack::nodeInScope(const char* name, ProcedureNode::NodeType nt) const
{
	for (int n=0; n<stack_.nItems(); ++n)
	{
		RefListIterator<ProcedureNode,bool> scopeIterator(stack_.constAt(n));
		while (ProcedureNode* node = scopeIterator.iterate())
		{
			if (DissolveSys::sameString(node->name(), name))
			{
				// Check type
				if (nt == ProcedureNode::nNodeTypes) return node;
				else if (node->type() == nt) return node;
			}
		}
	}

	return NULL;
}

// Return named node if known, and which matches the (optional) type given
ProcedureNode* NodeScopeStack::node(const char* name, ProcedureNode::NodeType nt) const
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
