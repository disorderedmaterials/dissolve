/*
	*** Node Scope
	*** src/procedure/nodescope.h
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

#ifndef DISSOLVE_PROCEDURE_NODESCOPE_H
#define DISSOLVE_PROCEDURE_NODESCOPE_H

#include "procedure/nodes/node.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
/* none */

// Node Scope
class NodeScope : public ListItem<NodeScope>
{
	public:
	// Constructor
	NodeScope(ProcedureNode::NodeContext context);
	// Destructor
	~NodeScope();


	/*
	 * Node Context / Content
	 */
	private:
	// Context for the scope
	ProcedureNode::NodeContext context_;
	// Nodes within the scope
	RefList<ProcedureNode> nodes_;

	public:
	// Return the current context
	ProcedureNode::NodeContext context() const;
	// Add new node to the scope
	bool add(ProcedureNode* node);


	/*
	 * Node Search
	 */
	public:
	// Return named node if known, and which matches the (optional) type given
	ProcedureNode* node(const char* name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
};

#endif
