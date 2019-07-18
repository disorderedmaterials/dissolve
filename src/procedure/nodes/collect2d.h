/*
	*** Procedure Node - Collect2D
	*** src/procedure/nodes/collect2d.h
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

#ifndef DISSOLVE_PROCEDURENODE_COLLECT2D_H
#define DISSOLVE_PROCEDURENODE_COLLECT2D_H

#include "procedure/nodes/node.h"
#include "base/charstring.h"
#include "math/histogram2d.h"
#include "templates/reflist.h"

// Forward Declarations
class CalculateProcedureNode;
class SequenceProcedureNode;
class LineParser;
class NodeContextStack;

// Procedure Node - Collect2D
class Collect2DProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	Collect2DProcedureNode(CalculateProcedureNode* xObservable = NULL, CalculateProcedureNode* yObservable = NULL, double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0, double yMax = 10.0, double yBinWidth = 0.05);
	// Destructor
	~Collect2DProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified usage type is allowed for this node
	bool isUsageTypeAllowed(ProcedureNode::NodeUsageType usageType);


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Collect2DNodeKeyword { EndCollect2DKeyword, QuantityXKeyword, QuantityYKeyword, RangeXKeyword, RangeYKeyword, SubCollectKeyword, nCollect2DNodeKeywords };
	// Return enum option info for Collect2DNodeKeyword
	static EnumOptions<Collect2DNodeKeyword> collect2DNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Observable to bin along x
	CalculateProcedureNode* xObservable_;
	// Observable to bin along y
	CalculateProcedureNode* yObservable_;
	// Histogram in which to accumulate data
	Histogram2D* histogram_;
	// X range minimum
	double xMinimum_;
	// X range maximum
	double xMaximum_;
	// X bin width
	double xBinWidth_;
	// Y range minimum
	double yMinimum_;
	// Y range maximum
	double yMaximum_;
	// Y bin width
	double yBinWidth_;

	public:
	// Return accumulated data
	const Data2D& accumulatedData() const;
	// Return x range minimum
	double xMinimum() const;
	// Return x range maximum
	double xMaximum() const;
	// Return x bin width
	double xBinWidth() const;
	// Return y range minimum
	double yMinimum() const;
	// Return y range maximum
	double yMaximum() const;
	// Return y bin width
	double yBinWidth() const;


	/*
	 * Branches
	 */
	private:
	// Branch for subcollection (if defined), run if the target quantity is successfully binned
	SequenceProcedureNode* subCollectBranch_;

	public:
	// Add and return subcollection sequence branch
	SequenceProcedureNode* addSubCollectBranch();
	// Add specified node to subcollection sequence
	void addToSubCollectBranch(ProcedureNode* node);


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
	bool read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
