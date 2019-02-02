/*
	*** Analysis Node - Collect3D
	*** src/analyse/nodes/collect3d.h
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
	// Constructors
	AnalysisCollect3DNode(AnalysisCalculateNode* xObservable = NULL, AnalysisCalculateNode* yObservable = NULL, AnalysisCalculateNode* zObservable = NULL, double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0, double yMax = 10.0, double yBinWidth = 0.05, double zMin = 0.0, double zMax = 10.0, double zBinWidth = 0.05);
	AnalysisCollect3DNode(AnalysisCalculateNode* xyzObservable, double xMin = 0.0, double xMax = 10.0, double xBinWidth = 0.05, double yMin = 0.0, double yMax = 10.0, double yBinWidth = 0.05, double zMin = 0.0, double zMax = 10.0, double zBinWidth = 0.05);
	// Destructor
	~AnalysisCollect3DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Collect3DNodeKeyword { EndCollect3DKeyword, QuantityXYZKeyword, QuantityXKeyword, QuantityYKeyword, QuantityZKeyword, RangeXKeyword, RangeYKeyword, RangeZKeyword, nCollect3DNodeKeywords };
	// Convert string to control keyword
	static Collect3DNodeKeyword collect3DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* collect3DNodeKeyword(Collect3DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Observable to bin along x
	AnalysisCalculateNode* xObservable_;
	// Observable to bin along y
	AnalysisCalculateNode* yObservable_;
	// Observable to bin along z
	AnalysisCalculateNode* zObservable_;
	// Observable to use for bins along x, y, and z
	AnalysisCalculateNode* xyzObservable_;
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
	bool read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
