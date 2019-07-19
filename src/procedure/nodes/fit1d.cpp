/*
	*** Procedure Node - Fit1D
	*** src/procedure/nodes/fit1d.cpp
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

#include "procedure/nodes/fit1d.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodescopestack.h"
#include "classes/configuration.h"
#include "expression/expression.h"
#include "expression/variable.h"
#include "math/mc.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Fit1DProcedureNode::Fit1DProcedureNode(Collect1DProcedureNode* target) : ProcedureNode(ProcedureNode::Fit1DNode)
{
	dataNode_.addAllowableNodeType(ProcedureNode::Process1DNode);

	// Create persistent 'x' variable for Expression
	xVariable_ = equation_.createVariable("x", true);

	dataNode_ = target;
	saveData_ = false;
}

// Destructor
Fit1DProcedureNode::~Fit1DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Fit1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Fit1DNodeKeyword
EnumOptions<Fit1DProcedureNode::Fit1DNodeKeyword> Fit1DProcedureNode::fit1DNodeKeywords()
{
	static EnumOptionsList Fit1DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Fit1DProcedureNode::ConstantKeyword,		"Constant") <<
		EnumOption(Fit1DProcedureNode::EndFit1DKeyword,		"EndFit1D") <<
		EnumOption(Fit1DProcedureNode::EquationKeyword,		"Equation") <<
		EnumOption(Fit1DProcedureNode::FitKeyword,		"Fit") <<
		EnumOption(Fit1DProcedureNode::MethodKeyword,		"Method") <<
		EnumOption(Fit1DProcedureNode::SaveKeyword,		"Save") <<
		EnumOption(Fit1DProcedureNode::SourceDataKeyword,	"SourceData");

	static EnumOptions<Fit1DProcedureNode::Fit1DNodeKeyword> options("Fit1DNodeKeyword", Fit1DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Fitting cost function
double Fit1DProcedureNode::equationCost(const Array<double>& alpha)
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
void Fit1DProcedureNode::setSaveData(bool on)
{
	saveData_ = on;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Fit1DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Fit1DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Fit1DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Copy reference data from the associated node
	Collect1DProcedureNode* collect1DNode;
	Process1DProcedureNode* process1DNode;
	switch (dataNode_.type())
	{
		case (ProcedureNode::Collect1DNode):
			collect1DNode = dynamic_cast<Collect1DProcedureNode*>(dataNode_.node());
			if (collect1DNode) return Messenger::error("Failed to cast dataNode_ into an Collect1DProcedureNode.\n");
			referenceData_ = collect1DNode->accumulatedData();
			break;
		case (ProcedureNode::Process1DNode):
			process1DNode = dynamic_cast<Process1DProcedureNode*>(dataNode_.node());
			if (process1DNode) return Messenger::error("Failed to cast dataNode_ into an Collect1DProcedureNode.\n");
			referenceData_ = process1DNode->processedData();
			break;
		case (ProcedureNode::nNodeTypes):
			return Messenger::error("No node type set in Fit1DProcedureNode::finalise().\n");
			break;
		default:
			return Messenger::error("No suitable data to extract from a node of type '%s'.\n", ProcedureNode::nodeTypes().keyword(dataNode_.type()));
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
		MonteCarloMinimiser<Fit1DProcedureNode> mcMinimiser(*this, &Fit1DProcedureNode::equationCost, true);
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
bool Fit1DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser may contain a node name for us
	if (parser.nArgs() == 2) setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Fit1D node '%s' to scope stack.\n", name());

	ExpressionVariable* var;

	// Read until we encounter the EndFit1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Fit1DNodeKeyword nk = fit1DNodeKeywords().enumeration(parser.argc(0));
		switch (nk)
		{
			case (Fit1DProcedureNode::ConstantKeyword):
				if (!equation_.createVariableWithValue(parser.argc(1), parser.argd(2), true)) return Messenger::error("Failed to create constant.\n");
				break;
			case (Fit1DProcedureNode::EndFit1DKeyword):
				return true;
			case (Fit1DProcedureNode::EquationKeyword):
				equationString_ = parser.argc(1);
				if (!equation_.generate(equationString_)) return Messenger::error("Failed to create expression.\n");
				break;
			case (Fit1DProcedureNode::FitKeyword):
				var = equation_.createVariableWithValue(parser.argc(1), parser.argd(2), true);
				if (!var) return Messenger::error("Failed to create variable '%s'.\n", parser.argc(1));
				fitTargets_.add(var);
				break;
			case (Fit1DProcedureNode::MethodKeyword):
				return false;
				break;
			case (Fit1DProcedureNode::SaveKeyword):
				saveData_ = parser.argb(1);
				break;
			case (Fit1DProcedureNode::SourceDataKeyword):
				if (!dataNode_.read(parser, 1, coreData, scopeStack)) return Messenger::error("Couldn't set source data for node.\n");
				break;
			case (Fit1DProcedureNode::nFit1DNodeKeywords):
				return Messenger::error("Unrecognised Fit1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Fit1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool Fit1DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
