/*
	*** Procedure Node - Collect3D
	*** src/procedure/nodes/collect3d.h
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

#ifndef DISSOLVE_PROCEDURENODE_COLLECT3D_H
#define DISSOLVE_PROCEDURENODE_COLLECT3D_H

#include "procedure/nodes/node.h"
#include "base/charstring.h"
#include "math/histogram3d.h"
#include "templates/reflist.h"

// Forward Declarations
class CalculateProcedureNode;
class SequenceProcedureNode;
class LineParser;
class NodeContextStack;

// Procedure Node - Collect3D
class Collect3DProcedureNode : public ProcedureNode
{
	public:
	// Constructors
	Collect3DProcedureNode(CalculateProcedureNode* xObservable = NULL, CalculateProcedureNode* yObservable = NULL, CalculateProcedureNode* zObservable = NULL, double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0, double yMax = 10.0, double yBinWidth = 0.05, double zMin = 0.0, double zMax = 10.0, double zBinWidth = 0.05);
	Collect3DProcedureNode(CalculateProcedureNode* xyzObservable, double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0, double yMax = 10.0, double yBinWidth = 0.05, double zMin = 0.0, double zMax = 10.0, double zBinWidth = 0.05);
	// Destructor
	~Collect3DProcedureNode();


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
	enum Collect3DNodeKeyword { EndCollect3DKeyword, QuantityXYZKeyword, QuantityXKeyword, QuantityYKeyword, QuantityZKeyword, RangeXKeyword, RangeYKeyword, RangeZKeyword, SubCollectKeyword, nCollect3DNodeKeywords };
	// Return enum option info for Collect3DNodeKeyword
	static EnumOptions<Collect3DNodeKeyword> collect3DNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Observable to bin along x
	CalculateProcedureNode* xObservable_;
	// Observable to bin along y
	CalculateProcedureNode* yObservable_;
	// Observable to bin along z
	CalculateProcedureNode* zObservable_;
	// Observable to use for bins along x, y, and z
	CalculateProcedureNode* xyzObservable_;
	// Histogram in which to accumulate data
	Histogram3D* histogram_;
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
	// Z range minimum
	double zMinimum_;
	// Z range maximum
	double zMaximum_;
	// Z bin width
	double zBinWidth_;

	public:
	// Return accumulated data
	const Data3D& accumulatedData() const;
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
	// Return z range minimum
	double zMinimum() const;
	// Return z range maximum
	double zMaximum() const;
	// Return z bin width
	double zBinWidth() const;


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
