/*
	*** Procedure Node - Sum1D
	*** src/procedure/nodes/sum1d.h
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

#ifndef DISSOLVE_PROCEDURENODE_SUM1D_H
#define DISSOLVE_PROCEDURENODE_SUM1D_H

#include "procedure/nodes/node.h"
#include "math/sampleddouble.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class Process1DProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Sum1D
class Sum1DProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	Sum1DProcedureNode(const Process1DProcedureNode* target = NULL);
	// Destructor
	~Sum1DProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified context is relevant for this node type
	bool isContextRelevant(ProcedureNode::NodeContext context);


	/*
	 * Data
	 */
	private:
	// Process1D node that we are targetting (retrieved from keyword 'SourceData')
	const Process1DProcedureNode* processNode_;
	// Calculated sums (stored in processing data list)
	SampledDouble sum_[3];

	public:
	// Return calculated sum specified
	const SampledDouble& sum(int index) const;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
};

#endif
