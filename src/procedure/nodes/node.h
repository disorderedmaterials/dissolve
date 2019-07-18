/*
	*** Analysis Base Node
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

#ifndef DISSOLVE_PROCEDURENODE_NODE_H
#define DISSOLVE_PROCEDURENODE_NODE_H

#include "base/charstring.h"
#include "base/enumoptions.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class CoreData;
class GenericList;
class LineParser;
class Procedure;
class ProcessPool;
class Site;
class NodeContextStack;

// Analysis Base Node
class ProcedureNode : public ListItem<ProcedureNode>
{
	public:
	// Node Types
	enum NodeType { CalculateNode, Collect1DNode, Collect2DNode, Collect3DNode, DynamicSiteNode, ExcludeNode, Fit1DNode, Process1DNode, Process2DNode, Process3DNode, SelectNode, SequenceNode, nNodeTypes };
	// Return enum option info for NodeType
	static EnumOptions<NodeType> nodeTypes();
	// Constructor
	ProcedureNode(NodeType nodeType);
	// Destructor
	virtual ~ProcedureNode();


	/*
	 * Data
	 */
	private:
	// Node type
	NodeType type_;
	// Node name
	CharString name_;
	// Node nice name
	CharString niceName_;
	// Procedure in which the node exists
	Procedure* procedure_;

	public:
	// Return node type
	NodeType type() const;
	// Set node name (and nice name)
	void setName(const char* name);
	// Return node name
	const char* name() const;
	// Return node nice name
	const char* niceName() const;
	// Set parent Procedure in which the node exists
	void setProcedure(Procedure* proc);
	// Return parent Procedure in which the node exists
	Procedure* procedure() const;


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
	virtual bool read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack) = 0;
	// Write structure to specified LineParser
	virtual bool write(LineParser& parser, const char* prefix) = 0;
};

#endif
