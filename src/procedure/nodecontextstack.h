/*
	*** Node Context Stack
	*** src/procedure/nodecontextstack.h
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

#ifndef DISSOLVE_PROCEDURE_NODECONTEXTSTACK_H
#define DISSOLVE_PROCEDURE_NODECONTEXTSTACK_H

#include "procedure/nodes/node.h"
#include "base/charstring.h"
#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
/* none */

// Node Context Stack
class NodeContextStack
{
	public:
	// Constructor
	NodeContextStack();
	// Destructor
	~NodeContextStack();


	/*
	 * Context / Reference Stacks
	 */
	private:
	// Counter for SelectNode added to the stack at any point
	int nSelectNodesAdded_;
	// Context stack
	Array< RefList<ProcedureNode,bool> > stack_;
	// References for all nodes added
	RefList<ProcedureNode,bool> nodes_;

	public:
	// Clear all layers from stack
	void clear();
	// Push new context layer on to the stack
	void push();
	// Pop topmost context layer from the stack
	bool pop();
	// Add new node to the topmost context layer
	bool add(ProcedureNode* node);


	/*
	 * Node Search
	 */
	public:
	// Return next available generic name for a SelectNode
	const char* nextSelectName() const;
	// Return named node if it is currently in scope, and optionally matches the type given
	ProcedureNode* nodeInScope(const char* name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
	// Return named node if known, and which matches the (optional) type given
	ProcedureNode* node(const char* name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
};

#endif
