/*
	*** Procedure Node
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

// Return enum option info for NodeType
EnumOptions<ProcedureNode::NodeType> ProcedureNode::nodeTypes()
{
	static EnumOptionsList NodeTypeKeywords = EnumOptionsList() <<
		EnumOption(ProcedureNode::BoxNode,		"Box") <<
		EnumOption(ProcedureNode::CalculateNode,	"Calculate") <<
		EnumOption(ProcedureNode::Collect1DNode,	"Collect1D") <<
		EnumOption(ProcedureNode::Collect2DNode,	"Collect2D") <<
		EnumOption(ProcedureNode::Collect3DNode,	"Collect3D") <<
		EnumOption(ProcedureNode::DynamicSiteNode,	"DynamicSite") <<
		EnumOption(ProcedureNode::ExcludeNode,		"Exclude") <<
		EnumOption(ProcedureNode::Fit1DNode,		"Fit1D") <<
		EnumOption(ProcedureNode::ParametersNode,	"Parameters") <<
		EnumOption(ProcedureNode::Process1DNode,	"Process1D") <<
		EnumOption(ProcedureNode::Process2DNode,	"Process2D") <<
		EnumOption(ProcedureNode::Process3DNode,	"Process3D") <<
		EnumOption(ProcedureNode::SelectNode,		"Select") <<
		EnumOption(ProcedureNode::SequenceNode,		"Sequence");

	static EnumOptions<ProcedureNode::NodeType> options("NodeType", NodeTypeKeywords, ProcedureNode::nNodeTypes);

	return options;
}

// Return enum option info for NodeContext
EnumOptions<ProcedureNode::NodeContext> ProcedureNode::nodeContexts()
{
	static EnumOptionsList NodeContextKeywords = EnumOptionsList() << 
		EnumOption(ProcedureNode::NoContext,			"None") <<
		EnumOption(ProcedureNode::AnalysisContext,		"Analysis") <<
		EnumOption(ProcedureNode::GenerationContext,		"Generation");

	static EnumOptions<ProcedureNode::NodeContext> options("NodeContext", NodeContextKeywords, ProcedureNode::NoContext);

	return options;
}

// Constructor
ProcedureNode::ProcedureNode(ProcedureNode::NodeType nodeType) : ListItem<ProcedureNode>()
{
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
 * Identity
 */

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

/*
 * Execution
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
