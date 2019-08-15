/*
	*** Procedure Node - Collect1D
	*** src/procedure/nodes/collect1d.h
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

#ifndef DISSOLVE_PROCEDURENODE_COLLECT1D_H
#define DISSOLVE_PROCEDURENODE_COLLECT1D_H

#include "procedure/nodes/node.h"
#include "base/charstring.h"
#include "math/histogram1d.h"
#include "templates/reflist.h"

// Forward Declarations
class CalculateProcedureNode;
class SequenceProcedureNode;
class LineParser;
class NodeScopeStack;

// Procedure Node - Collect1D
class Collect1DProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	Collect1DProcedureNode(CalculateProcedureNode* observable = NULL, double rMin = 0.0, double rMax = 10.0, double binWidth = 0.05);
	// Destructor
	~Collect1DProcedureNode();


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
	enum Collect1DNodeKeyword { EndCollect1DKeyword, QuantityXKeyword, RangeXKeyword, SubCollectKeyword, nCollect1DNodeKeywords };
	// Return enum option info for Collect1DNodeKeyword
	static EnumOptions<Collect1DNodeKeyword> collect1DNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Observable to bin
	CalculateProcedureNode* observable_;
	// Histogram in which to accumulate data
	Histogram1D* histogram_;
	// Range minimum
	double minimum_;
	// Range maximum
	double maximum_;
	// Bin width
	double binWidth_;

	public:
	// Return accumulated data
	const Data1D& accumulatedData() const;
	// Return range minimum
	double minimum() const;
	// Return range maximum
	double maximum() const;
	// Return bin width
	double binWidth() const;


	/*
	 * Branches
	 */
	private:
	// Branch for subcollection (if defined), run if the target quantity is successfully binned
	SequenceProcedureNode* subCollectBranch_;

	public:
	// Add and return subcollection sequence branch
	SequenceProcedureNode* addSubCollectBranch(ProcedureNode::NodeContext context);
	// Return whether this node has a branch
	bool hasBranch() const;
	// Return SequenceNode for the branch (if it exists)
	SequenceProcedureNode* branch();


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
