/*
	*** Analysis Base Node
	*** src/analyse/nodes/node.h
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

#ifndef DISSOLVE_ANALYSISNODE_H
#define DISSOLVE_ANALYSISNODE_H

#include "base/charstring.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class GenericList;
class LineParser;
class ProcessPool;
class Site;
class NodeContextStack;

// Analysis Base Node
class AnalysisNode : public ListItem<AnalysisNode>
{
	public:
	// Available Node Types
	enum NodeType { CalculateNode, Collect1DNode, Collect2DNode, Collect3DNode, DynamicSelectNode, ExcludeNode, Normalise1DNode, Normalise2DNode, Normalise3DNode, SelectNode, SequenceNode, nNodeTypes };
	// Convert string to node type
	static NodeType nodeType(const char* s);
	// Convert node type to string
	static const char* nodeType(NodeType nt);
	// Constructor
	AnalysisNode(NodeType nodeType);
	// Destructor
	virtual ~AnalysisNode();


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

	public:
	// Return node type
	NodeType type() const;
	// Set node name (and nice name)
	void setName(const char* name);
	// Return node name
	const char* name() const;
	// Return node nice name
	const char* niceName() const;


	/*
	 * Execute
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
	virtual bool read(LineParser& parser, NodeContextStack& contextStack) = 0;
	// Write structure to specified LineParser
	virtual bool write(LineParser& parser, const char* prefix) = 0;
};

#endif
