/*
	*** Procedure Node
	*** src/procedure/nodes/node.h
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

#ifndef DISSOLVE_PROCEDURENODE_H
#define DISSOLVE_PROCEDURENODE_H

#include "keywords/list.h"
#include "base/charstring.h"
#include "base/enumoptions.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class CoreData;
class GenericList;
class LineParser;
class NodeScopeStack;
class Procedure;
class ProcessPool;
class SequenceProcedureNode;
class Site;

// Procedure Node
class ProcedureNode : public ListItem<ProcedureNode>
{
	public:
	// Node Types
	enum NodeType { AddSpeciesNode, BoxNode, CalculateNode, Collect1DNode, Collect2DNode, Collect3DNode, DynamicSiteNode, ExcludeNode, Fit1DNode, ParametersNode, Process1DNode, Process2DNode, Process3DNode, SelectNode, SequenceNode, nNodeTypes };
	// Return enum option info for NodeType
	static EnumOptions<NodeType> nodeTypes();
	// Node Contexts
	enum NodeContext { NoContext = 0, AnalysisContext = 1, GenerationContext = 2 };
	// Return enum option info for NodeContext
	static EnumOptions<NodeContext> nodeContexts();
	// Constructor
	ProcedureNode(NodeType nodeType);
	// Destructor
	virtual ~ProcedureNode();


	/*
	 * Identity
	 */
	protected:
	// Node type
	NodeType type_;
	// Node name
	CharString name_;
	// Node nice name
	CharString niceName_;

	public:
	// Return node type
	NodeType type() const;
	// Return whether specified context is relevant for this node type
	virtual bool isContextRelevant(NodeContext context) = 0;
	// Set node name (and nice name)
	void setName(const char* name);
	// Return node name
	const char* name() const;
	// Return node nice name
	const char* niceName() const;


	/*
	 * Keywords
	 */
	protected:
	// Keywords for this node
	KeywordList keywords_;

	private:
	// Set-up keywords for this node
	virtual void setUpKeywords() = 0;

	public:
	// Return keywords for this node
	const KeywordList& keywords() const;


	/*
	 * Branch
	 */
	public:
	// Return whether this node has a branch
	virtual bool hasBranch() const;
	// Return SequenceNode for the branch (if it exists)
	virtual SequenceProcedureNode* branch();


	/*
	 * Execution
	 */
	public:
	// Node execution result
	enum NodeExecutionResult { Failure, Success, SomethingElse };
	// Prepare any necessary data, ready for execution
	virtual bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	virtual NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList) = 0;
	// Finalise any necessary data after execution
	virtual bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	virtual bool read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack) = 0;
	// Write structure to specified LineParser
	virtual bool write(LineParser& parser, const char* prefix) = 0;
};

#endif
