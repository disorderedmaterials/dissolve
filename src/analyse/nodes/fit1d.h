/*
	*** Analysis Node - Fit1D
	*** src/analyse/nodes/fit1d.h
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

#ifndef DISSOLVE_ANALYSISFIT1D_H
#define DISSOLVE_ANALYSISFIT1D_H

#include "analyse/nodes/node.h"
#include "analyse/nodes/nodereference.h"
#include "math/data1d.h"
#include "expression/expression.h"
#include "base/charstring.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisCollect1DNode;
class AnalysisSelectNode;
class Data1D;
class LineParser;
class NodeContextStack;

// Analysis Node - Fit1D
class AnalysisFit1DNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisFit1DNode(AnalysisCollect1DNode* target = NULL);
	// Destructor
	~AnalysisFit1DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Fit1DNodeKeyword { ConstantKeyword, EndFit1DKeyword, EquationKeyword, FitKeyword, MethodKeyword, SaveKeyword, SourceData, nFit1DNodeKeywords };
	// Convert string to control keyword
	static Fit1DNodeKeyword fit1DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* fit1DNodeKeyword(Fit1DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Node containing source data to fit
	AnalysisNodeReference dataNode_;
	// Fit equation string
	CharString equationString_;
	// Fit equation object
	Expression equation_;
	// Data against which to fit
	Data1D referenceData_;
	// X variable for equation
	ExpressionVariable* xVariable_;
	// List of variables which we are fitting
	RefList<ExpressionVariable, bool> fitTargets_;
	// Whether to save data after normalisation
	bool saveData_;

	private:
	// Fitting cost function
	double equationCost(const Array<double>& alpha);

	public:
	// Return fitted data
	const Data1D& fittedData() const;
	// Set whether to save processed data
	void setSaveData(bool on);


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
