/*
	*** Keyword - Node
	*** src/keywords/node.h
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

#ifndef DISSOLVE_KEYWORD_NODE_H
#define DISSOLVE_KEYWORD_NODE_H

#include "keywords/data.h"
#include "keywords/base.h"
#include "procedure/nodes/node.h"
#include "base/lineparser.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with ProcedureNode base class
class NodeKeywordBase
{
	public:
	// Constructor
	NodeKeywordBase(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType);
	// Destructor
	virtual ~NodeKeywordBase();


	/*
	 * Parent Node
	 */
	private:
	// Parent ProcedureNode
	ProcedureNode* parentNode_;

	public:
	// Parent ProcedureNode
	ProcedureNode* parentNode() const;


	/*
	 * Target Node Type
	 */
	private:
	// Target node type to allow
	ProcedureNode::NodeType nodeType_;

	public:
	// Return target node type to allow
	ProcedureNode::NodeType nodeType() const;


	/*
	 * Set
	 */
	public:
	// Set the target node
	virtual bool setNode(ProcedureNode* node) = 0;
};

// Keyword with ProcedureNode
template <class N> class NodeKeyword : public NodeKeywordBase, public KeywordData<N*>
{
	public:
	// Constructor
	NodeKeyword(ProcedureNode* parentNode, ProcedureNode::NodeType nodeType, N* node) : NodeKeywordBase(parentNode, nodeType), KeywordData<N*>(KeywordBase::NodeData, node)
	{
	}
	// Destructor
	~NodeKeyword()
	{
	}


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments()
	{
		return 1;
	}
	// Return maximum number of arguments accepted
	int maxArguments()
	{
		return 1;
	}
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData)
	{
		if (!parentNode()) return Messenger::error("Can't read keyword %s since the parent ProcedureNode has not been set.\n", KeywordBase::keyword());

		// Locate the named node in scope - don't prune by type yet (we'll check that in setNode())
		ProcedureNode* node = parentNode()->nodeInScope(parser.argc(startArg));
		if (!node) return Messenger::error("Node '%s' given to keyword %s doesn't exist.\n", parser.argc(startArg), KeywordBase::keyword());

		return setNode(node);
	}
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix)
	{
		if (!parser.writeLineF("%s%s  '%s'\n", prefix, KeywordBase::keyword(), KeywordData<N*>::data_->name())) return false;

		return true;
	}


	/*
	 * Set
	 */
	public:
	// Set the target node
	bool setNode(ProcedureNode* node)
	{
		if (!node) return false;

		if (node->type() != nodeType()) return Messenger::error("Node '%s' is of type %s, but the %s keyword requires a node of type %s.\n", node->name(), ProcedureNode::nodeTypes().keyword(node->type()), KeywordBase::keyword(), ProcedureNode::nodeTypes().keyword(nodeType()));

		KeywordData<N*>::data_ = dynamic_cast<N*>(node);

		KeywordData<N*>::set_ = true;

		return true;
	}


	/*
	 * Object Management
	 */
	protected:
};

#endif

