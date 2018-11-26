/*
	*** Node Context Stack
	*** src/analyse/nodecontextstack.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_NODECONTEXTSTACK_H
#define DISSOLVE_NODECONTEXTSTACK_H

#include "analyse/nodes/node.h"
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
	Array< RefList<AnalysisNode,bool> > stack_;
	// References for all nodes added
	RefList<AnalysisNode,bool> nodes_;

	public:
	// Clear all layers from stack
	void clear();
	// Push new context layer on to the stack
	void push();
	// Pop topmost context layer from the stack
	bool pop();
	// Add new node to the topmost context layer
	bool add(AnalysisNode* node);


	/*
	 * Node Search
	 */
	public:
	// Return next available generic name for a SelectNode
	const char* nextSelectName() const;
	// Return named node if it is currently in scope, and optionally matches the type given
	AnalysisNode* nodeInScope(const char* name, AnalysisNode::NodeType nt = AnalysisNode::nNodeTypes) const;
	// Return named node if known, and which matches the (optional) type given
	AnalysisNode* node(const char* name, AnalysisNode::NodeType nt = AnalysisNode::nNodeTypes) const;
};

#endif
