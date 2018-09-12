/*
	*** Analysis Node - Collect3D
	*** src/analyse/nodes/collect3d.h
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

#ifndef DISSOLVE_ANALYSISCOLLECT3D_H
#define DISSOLVE_ANALYSISCOLLECT3D_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"
#include "math/histogram3d.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisCalculateNode;
class LineParser;
class NodeContextStack;

// Analysis Node - Collect3D
class AnalysisCollect3DNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisCollect3DNode(AnalysisCalculateNode* observable = NULL, double rMin = 0.0, double rMax = 10.0, double binWidth = 0.05);
	// Destructor
	~AnalysisCollect3DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Collect3DNodeKeyword { EndCollect3DKeyword, QuantityXKeyword, QuantityYKeyword, QuantityZKeyword, RangeXKeyword, RangeYKeyword, RangeZKeyword, nCollect3DNodeKeywords };
	// Convert string to control keyword
	static Collect3DNodeKeyword collect3DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* collect3DNodeKeyword(Collect3DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Observable to bin
	AnalysisCalculateNode* observable_;
	// Histogram in which to accumulate data
	Histogram3D* histogram_;
	// Range minimum
	double minimum_;
	// Range maximum
	double maximum_;
	// Bin width
	double binWidth_;

	public:
	// Return accumulated data
	const Data3D& accumulatedData() const;
	// Return range minimum
	double minimum() const;
	// Return range maximum
	double maximum() const;
	// Return bin width
	double binWidth() const;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
