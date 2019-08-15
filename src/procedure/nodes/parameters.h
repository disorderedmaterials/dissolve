/*
	*** Procedure Node - Parameters
	*** src/procedure/nodes/parameters.h
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

#ifndef DISSOLVE_PROCEDURENODE_PARAMETERS_H
#define DISSOLVE_PROCEDURENODE_PARAMETERS_H

#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/array.h"

// Forward Declarations
class ExpressionVariable;

// Parameters Node
class ParametersProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	ParametersProcedureNode();
	// Destructor
	~ParametersProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified context is relevant for this node type
	bool isContextRelevant(ProcedureNode::NodeContext context);


	/*
	 * Node Keywords
	 */
	private:
	// Set up keywords for node
	void setUpKeywords();

	public:
	// Node Keywords
	enum ParametersNodeKeyword { DoubleKeyword, IntegerKeyword, EndParametersKeyword, nParametersNodeKeywords };
	// Return enum option info for ParametersNodeKeyword
	static EnumOptions<ParametersNodeKeyword> parametersNodeKeywords();


	/*
	 * Parameters
	 */
	private:
	// List of defined parameters (as base ExpressionNode type)
	List<ExpressionNode> parameters_;
	// Reference list of parameres (as ExpressionVariables)
	RefList<ExpressionVariable> parameterReferences_;

	private:
	// Add new parameter for Procedure
	bool addParameter(const char* name, ExpressionValue value);

	public:
	// Return whether this node has the named parameter specified
	ExpressionVariable* hasParameter(const char* name);
	// Return references to all parameters for this node
	RefList<ExpressionVariable> parameterReferences() const;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
