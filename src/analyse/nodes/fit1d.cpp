/*
	*** Analysis Node - Fit1D
	*** src/analyse/nodes/fit1d.cpp
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

#include "analyse/nodes/fit1d.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/process1d.h"
#include "analyse/nodecontextstack.h"
#include "classes/configuration.h"
#include "expression/expression.h"
#include "expression/variable.h"
#include "math/mc.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalysisFit1DNode::AnalysisFit1DNode(AnalysisCollect1DNode* target) : AnalysisNode(AnalysisNode::Fit1DNode)
{
	dataNode_.addAllowableNodeType(AnalysisNode::Process1DNode);

	// Create persistent 'x' variable for Expression
	xVariable_ = equation_.createVariable("x", true);

	dataNode_ = target;
	saveData_ = false;
}

// Destructor
AnalysisFit1DNode::~AnalysisFit1DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* Fit1DNodeKeywords[] = { "Constant", "EndFit1D", "Equation", "Fit", "Method", "Save", "SourceData" };

// Convert string to node keyword
AnalysisFit1DNode::Fit1DNodeKeyword AnalysisFit1DNode::fit1DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisFit1DNode::nFit1DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Fit1DNodeKeywords[nk])) return (AnalysisFit1DNode::Fit1DNodeKeyword) nk;

	return AnalysisFit1DNode::nFit1DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisFit1DNode::fit1DNodeKeyword(AnalysisFit1DNode::Fit1DNodeKeyword nk)
{
	return Fit1DNodeKeywords[nk];
}

/*
 * Data
 */

// Fitting cost function
double AnalysisFit1DNode::equationCost(const Array<double>& alpha)
{
	// We assume that the minimiser has 'pokeBeforeCost' set, so our Expression's variables are up-to-date with new test values.
	double cost = 0.0;
	const Array<double>& x = referenceData_.xAxis();
	const Array<double>& y = referenceData_.values();
	double equationY;
	bool success;
	for (int n=0; n<referenceData_.nValues(); ++n)
	{
		// Set axis value
		xVariable_->set(x.constAt(n));

		// Evaluate expression
		equationY = equation_.execute(success);

		// Sum squared error
		cost += (equationY - y.constAt(n))*(equationY - y.constAt(n));
	}

	cost /= referenceData_.nValues();

// 	printf("For A = %f c = %f, cost = %f\n", fitTargets_.first()->item->value(), fitTargets_.first()->next->item->value(), sqrt(cost));

	return sqrt(cost);
}

// Set whether to save processed data
void AnalysisFit1DNode::setSaveData(bool on)
{
	saveData_ = on;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisFit1DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisFit1DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisFit1DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Copy reference data from the associated node
	switch (dataNode_.type())
	{
		case (AnalysisNode::Collect1DNode):
			referenceData_ = ((AnalysisCollect1DNode*) dataNode_.node())->accumulatedData();
			break;
		case (AnalysisNode::Process1DNode):
			referenceData_ = ((AnalysisProcess1DNode*) dataNode_.node())->processedData();
			break;
		case (AnalysisNode::nNodeTypes):
			return Messenger::error("No node type set in AnalysisFit1DNode::finalise().\n");
			break;
		default:
			return Messenger::error("No suitable data to extract from a node of type '%s'.\n", AnalysisNode::nodeType(dataNode_.type()));
	}

	// Print equation info
	Messenger::print("Expression to fit is: %s\n", equationString_.get());
	RefListIterator<ExpressionVariable,bool> variableIterator(equation_.variables());
	while (ExpressionVariable* var = variableIterator.iterate())
	{
		if (var == xVariable_) Messenger::print("  %10s                (axis variable)\n", var->name());
		else if (fitTargets_.contains(var)) Messenger::print("  %10s = %e (fit)\n", var->name(), var->value());
		else Messenger::print("  %10s = %e (constant)\n", var->name(), var->value());
	}
	Messenger::print("\n");

	// Check number of variable parameters
	if (fitTargets_.nItems() > 0)
	{
		// Create a minimiser
		MonteCarloMinimiser<AnalysisFit1DNode> mcMinimiser(*this, &AnalysisFit1DNode::equationCost, true);
		mcMinimiser.setMaxIterations(1000);
		mcMinimiser.setStepSize(0.1);
// 		mcMinimiser.setMinStepSize(0.001);
		RefListIterator<ExpressionVariable,bool> targetIterator(fitTargets_);
		while (ExpressionVariable* var = targetIterator.iterate()) mcMinimiser.addTarget(var);

		mcMinimiser.minimise();

		Messenger::print("Optimised values:\n");
		targetIterator.restart();
		while (ExpressionVariable* var = targetIterator.iterate()) Messenger::print("  %10s = %e\n", var->name(), var->value());
	}

	// Generate final fit data
	// Retrieve / realise the data from the supplied list
	Data1D& data = GenericListHelper<Data1D>::realise(targetList, CharString("%s_%s", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag);

	data.setName(name());
	data.setObjectTag(CharString("%s//Fit1D//%s//%s", prefix, cfg->name(), name()));
	data.clear();

	double cost = 0.0;
	const Array<double>& x = referenceData_.xAxis();
	bool success;
	for (int n=0; n<referenceData_.nValues(); ++n)
	{
		// Set axis value
		xVariable_->set(x.constAt(n));

		// Add point
		data.addPoint(x.constAt(n), equation_.execute(success));
	}

	// Save data?
	if (saveData_ && procPool.isMaster())
	{
		LineParser parser;
		if (!parser.openOutput(CharString("%s_%s.fit", name(), cfg->name()))) return procPool.decideFalse();
		if (!parser.writeLineF("# Fit Equation : %s\n", equationString_.get())) return procPool.decideFalse();
		RefListIterator<ExpressionVariable,bool> variableIterator(equation_.variables());
		while (ExpressionVariable* var = variableIterator.iterate())
		{
			if (var == xVariable_)
			{
				if (!parser.writeLineF("#  %10s                (axis variable)\n", var->name())) return procPool.decideFalse();
			}
			else if (fitTargets_.contains(var))
			{
				if (!parser.writeLineF("#  %10s = %e (fit)\n", var->name(), var->value())) return procPool.decideFalse();
			}
			else if (!parser.writeLineF("#  %10s = %e (constant)\n", var->name(), var->value())) return procPool.decideFalse();
		}
		if (!data.save(parser)) return procPool.decideFalse();

		procPool.decideTrue();
	}
	else if (!procPool.decision()) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisFit1DNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Fit1D node '%s' to context stack.\n", name());

	ExpressionVariable* var;

	// Read until we encounter the EndFit1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Fit1DNodeKeyword nk = fit1DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (AnalysisFit1DNode::ConstantKeyword):
				if (!equation_.createVariableWithValue(parser.argc(1), parser.argd(2), true)) return Messenger::error("Failed to create constant.\n");
				break;
			case (AnalysisFit1DNode::EndFit1DKeyword):
				return true;
			case (AnalysisFit1DNode::EquationKeyword):
				equationString_ = parser.argc(1);
				if (!equation_.generate(equationString_)) return Messenger::error("Failed to create expression.\n");
				break;
			case (AnalysisFit1DNode::FitKeyword):
				var = equation_.createVariableWithValue(parser.argc(1), parser.argd(2), true);
				if (!var) return Messenger::error("Failed to create variable '%s'.\n", parser.argc(1));
				fitTargets_.add(var);
				break;
			case (AnalysisFit1DNode::MethodKeyword):
				return false;
				break;
			case (AnalysisFit1DNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (AnalysisFit1DNode::SourceData):
				if (!dataNode_.read(parser, 1, coreData, contextStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (AnalysisFit1DNode::nFit1DNodeKeywords):
				return Messenger::error("Unrecognised Fit1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Fit1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisFit1DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
