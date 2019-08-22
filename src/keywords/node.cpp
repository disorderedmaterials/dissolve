/*
	*** Keyword - Node
	*** src/keywords/node.cpp
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

#include "keywords/node.h"
#include "procedure/nodes/node.h"
#include "base/lineparser.h"

// Constructor
NodeKeyword::NodeKeyword(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType, ProcedureNode* node) : KeywordData<ProcedureNode*>(KeywordData::NodeValueData, node)
{
	parentNode_ = parentNode;
	nodeType_ = nodeType;
}

// Destructor
NodeKeyword::~NodeKeyword()
{
}

/*
 * Parent Node
 */

// Parent ProcedureNode
const ProcedureNode* NodeKeyword::parentNode() const
{
	return parentNode_;
}

/*
 * Target Node Type
 */

// Return target node type to allow
ProcedureNode::NodeType NodeKeyword::nodeType() const
{
	return nodeType_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int NodeKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	if (!parentNode_) return Messenger::error("Can't read keyword %s since the parent ProcedureNode has not been set.\n", keyword());

	// Locate the named node in scope - don't prune by type yet (we'll check that in setNode())
	ProcedureNode* node = parentNode_->nodeInScope(parser.argc(startArg));
	if (!node) return Messenger::error("Node '%s' given to keyword %s doesn't exist.\n", parser.argc(startArg), keyword());

	return setNode(node);
}

// Write keyword data to specified LineParser
bool NodeKeyword::write(LineParser& parser, const char* prefix)
{
	if (!parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_->name())) return false;

	return true;
}

/*
 * Set
 */

// Set the target node
bool NodeKeyword::setNode(ProcedureNode* node)
{
	if (!node) return false;

	if (node->type() != nodeType_) return Messenger::error("Node '%s' is of type %s, but the %s keyword requires a node of type %s.\n", node->name(), ProcedureNode::nodeTypes().keyword(node->type()), keyword(), ProcedureNode::nodeTypes().keyword(nodeType_));

	data_ = node;

	set_ = true;

	return true;
}

/*
 * Object Management
 */
