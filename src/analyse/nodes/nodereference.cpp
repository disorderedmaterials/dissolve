/*
	*** Analysis Node Reference
	*** src/analyse/nodes/nodereference.cpp
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

#include "analyse/nodes/nodereference.h"
#include "modules/analyse/analyse.h"
#include "module/list.h"
#include "classes/coredata.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisNodeReference::AnalysisNodeReference(AnalysisNode* node) : ListItem<AnalysisNodeReference>()
{
	node_ = node;
	for (int n=0; n<AnalysisNode::nNodeTypes; ++n) allowedTypes_[n] = false;
}

// Destructor
AnalysisNodeReference::~AnalysisNodeReference()
{
}

/*
 * Data
 */

// Return target node
AnalysisNode* AnalysisNodeReference::node()
{
	return node_;
}

// Return node type
AnalysisNode::NodeType AnalysisNodeReference::type() const
{
	return (node_ ? node_->type() : AnalysisNode::nNodeTypes);
}

// Add allowable node type
void AnalysisNodeReference::addAllowableNodeType(AnalysisNode::NodeType nt)
{
	allowedTypes_[nt] = true;
}

// Allow all node types
void AnalysisNodeReference::setAllowAllNodeTypes()
{
	for (int n=0; n<AnalysisNode::nNodeTypes; ++n) allowedTypes_[n] = true;
}

// Return if node pointer is NULL
bool AnalysisNodeReference::isNull() const
{
	return (node_ == NULL);
}

/*
 * Operators
 */

// Assignment operator
void AnalysisNodeReference::operator=(AnalysisNode* node)
{
	node_ = node;
}

// Assignment operator
void AnalysisNodeReference::operator=(const AnalysisNodeReference& nodeRef)
{
	node_ = nodeRef.node_;
	for (int n=0; n<AnalysisNode::nNodeTypes; ++n) allowedTypes_[n] = nodeRef.allowedTypes_[n];
}

/*
 * Read / Write second argument was provided we assume this is the name of an AnalyseModule
 */

// Read structure from specified LineParser
bool AnalysisNodeReference::read(LineParser& parser, int startArg, const CoreData& coreData, NodeContextStack& localStack)
{
	node_ = NULL;

	// If two arguments are provided, the second is the identifying name of an AnalyseModule
	if (parser.nArgs() == (startArg+2))
	{
		Module* module = coreData.findModule(parser.argc(startArg+1));
		if (!module) return Messenger::error("No Analyse module named '%s' exists.\n", parser.argc(startArg+1));
		if (!DissolveSys::sameString("Analyse", module->type())) return Messenger::error("Specified module '%s' must be an Analyse module.\n", parser.argc(startArg+1));

		// Found the target AnalyseModule, so cast it up and search for the named node in its Analyser
		AnalyseModule* analyseModule = (AnalyseModule*) module;
		node_ = analyseModule->analyserContextStack().node(parser.argc(startArg));
	}
	else node_ = localStack.node(parser.argc(startArg));

	// Did we find a node with the specified name?
	if (!node_) return Messenger::error("No node named '%s' exists in the Analyse module specified.\n", parser.argc(startArg));

	// Check the type of the node
	if (!allowedTypes_[node_->type()]) return Messenger::error("Node '%s' is not of the correct type.\n", node_->name());

	return (node_ != NULL);
}

// Write structure to specified LineParser
bool AnalysisNodeReference::write(LineParser& parser, const char* prefix)
{
}
