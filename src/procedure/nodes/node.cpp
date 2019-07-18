/*
	*** Analysis Base Node
	*** src/procedure/nodes/node.cpp
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

#include "procedure/nodes/node.h"
#include "classes/site.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

// Constructor
ProcedureNode::ProcedureNode(ProcedureNode::NodeType nodeType) : ListItem<ProcedureNode>()
{
	procedure_ = NULL;

	type_ = nodeType;

	// Assign default, unique name to the node
	static int nodeCount = 0;
	name_ = CharString("Node%04i", ++nodeCount);
}

// Destructor
ProcedureNode::~ProcedureNode()
{
}

/*
 * Node Types
 */

// Control keywords
const char* NodeTypes[] = { "Calculate", "Collect1D", "Collect2D", "Collect3D", "DynamicSite", "Exclude", "Fit1D", "Process1D", "Process2D", "Process3D", "Select", "Sequence" };

// Convert string to node type
ProcedureNode::NodeType ProcedureNode::nodeType(const char* s)
{
	for (int nt=0; nt < ProcedureNode::nNodeTypes; ++nt) if (DissolveSys::sameString(s, NodeTypes[nt])) return (ProcedureNode::NodeType) nt;

	return ProcedureNode::nNodeTypes;
}

// Convert node type to string
const char* ProcedureNode::nodeType(ProcedureNode::NodeType nt)
{
	return NodeTypes[nt];
}

// Return node type
ProcedureNode::NodeType ProcedureNode::type() const
{
	return type_;
}

// Set node name (and nice name)
void ProcedureNode::setName(const char* name)
{
	name_ = name;

	// Generate a nice name (i.e. no spaces, slashes etc.)
	niceName_ = DissolveSys::niceName(name_);
}

// Return node name
const char* ProcedureNode::name() const
{
	return name_.get();
}

// Return nice node name
const char* ProcedureNode::niceName() const
{
	return niceName_.get();
}

// Set parent Procedure in which the node exists
void ProcedureNode::setProcedure(Procedure* proc)
{
	procedure_ = proc;
}

// Return parent Procedure in which the node exists
Procedure* ProcedureNode::procedure() const
{
	return procedure_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool ProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool ProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}
