/*
	*** Node Scope Stack
	*** src/procedure/nodescopestack.h
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

#ifndef DISSOLVE_PROCEDURE_NODESCOPESTACK_H
#define DISSOLVE_PROCEDURE_NODESCOPESTACK_H

#include "procedure/nodes/node.h"
#include "procedure/nodescope.h"
#include "expression/expression.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
/* none */

// Node Scope Stack
class NodeScopeStack
{
	public:
	// Constructor
	NodeScopeStack();
	// Destructor
	~NodeScopeStack();


	/*
	 * Scope Stack
	 */
	private:
	// Scope stack
	List<NodeScope> stack_;
	// References for all nodes added
	RefList<ProcedureNode,bool> nodes_;
	// Counter for SelectNode added to the stack at any point
	int nSelectNodesAdded_;

	public:
	// Clear all scopes from stack
	void clear();
	// Push new scope with the specified context on to the stack
	void push(ProcedureNode::NodeContext context);
	// Pop topmost scope from the stack
	bool pop();
	// Add new node to the topmost scope
	bool add(ProcedureNode* node);
	// Return context of topmost scope
	ProcedureNode::NodeContext currentContext() const;


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


	/*
	 * Expression Parameters
	 */
	private:
	// List of parameters (as ExpressionVariables)
	List<ExpressionNode> parameters_;
	// Reference list of parameters (for passing to Expression::generate())
	RefList<ExpressionVariable,bool> parameterReferences_;

	public:
	// Add new parameter for Procedure
	ExpressionVariable* addParameter(const char* name, double value);
	// Return reference list of parameters (for passing to Expression::generate())
	RefList<ExpressionVariable,bool> parameterReferences() const;
};

#endif
