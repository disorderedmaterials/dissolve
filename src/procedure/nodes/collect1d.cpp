/*
	*** Procedure Node - Collect1D
	*** src/procedure/nodes/collect1d.cpp
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

#include "procedure/nodes/collect1d.h"
#include "procedure/nodescopestack.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/sequence.h"
#include "math/data1d.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Collect1DProcedureNode::Collect1DProcedureNode(CalculateProcedureNode* observable, double rMin, double rMax, double binWidth) : ProcedureNode(ProcedureNode::Collect1DNode)
{
	observable_ = observable;
	minimum_ = rMin;
	maximum_ = rMax;
	binWidth_ = binWidth;

	// Branches
	subCollectBranch_ = NULL;
}

// Destructor
Collect1DProcedureNode::~Collect1DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Collect1DNodeKeyword
EnumOptions<Collect1DProcedureNode::Collect1DNodeKeyword> Collect1DProcedureNode::collect1DNodeKeywords()
{
	static EnumOptionsList Collect1DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Collect1DProcedureNode::EndCollect1DKeyword,		"EndCollect1D") <<
		EnumOption(Collect1DProcedureNode::QuantityXKeyword,		"QuantityX",	1) <<
		EnumOption(Collect1DProcedureNode::RangeXKeyword,		"RangeX",	3) <<
		EnumOption(Collect1DProcedureNode::SubCollectKeyword,		"SubCollect");

	static EnumOptions<Collect1DProcedureNode::Collect1DNodeKeyword> options("Collect1DNodeKeyword", Collect1DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Return accumulated data
const Data1D& Collect1DProcedureNode::accumulatedData() const
{
	if (!histogram_)
	{
		Messenger::error("No histogram pointer set in Collect1DProcedureNode, so no accumulated data to return.\n");
		static Data1D dummy;
		return dummy;
	}

	return histogram_->accumulatedData();
}

// Return range minimum
double Collect1DProcedureNode::minimum() const
{
	return minimum_;
}

// Return range maximum
double Collect1DProcedureNode::maximum() const
{
	return maximum_;
}

// Return bin width
double Collect1DProcedureNode::binWidth() const
{
	return binWidth_;
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode* Collect1DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
	if (!subCollectBranch_) subCollectBranch_ = new SequenceProcedureNode(context);

	return subCollectBranch_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect1DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Construct our data name, and search for it in the supplied list
	CharString dataName("%s_%s_Bins", name(), cfg->niceName());
	bool created;
	Histogram1D& target = GenericListHelper<Histogram1D>::realise(targetList, dataName.get(), prefix, GenericItem::InRestartFileFlag, &created);
	if (created)
	{
		Messenger::printVerbose("One-dimensional histogram data for '%s' was not in the target list, so it will now be initialised...\n", name());
		target.initialise(minimum_, maximum_, binWidth_);
	}

	// Zero the current bins, ready for the new pass
	target.zeroBins();

	// Store a pointer to the data
	histogram_ = &target;

	// Prepare any branches
	if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList))) return false;

	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult Collect1DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!observable_)
	{
		Messenger::error("No CalculateProcedureNode pointer set in Collect1DProcedureNode '%s'.\n", name());
		return ProcedureNode::Failure;
	}
#endif
	// Bin the current value of the observable, and execute sub-collection branch on success
	if (histogram_->bin(observable_->value(0)) && subCollectBranch_) return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect1DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!histogram_)
	{
		Messenger::error("No Data1D pointer set in Collect1DProcedureNode '%s'.\n", name());
		return ProcedureNode::Failure;
	}
#endif
	// Accumulate the current binned data
	histogram_->accumulate();

	// Finalise any branches
	if (subCollectBranch_ && (!subCollectBranch_->finalise(procPool, cfg, prefix, targetList))) return false;

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Collect1DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser must also contain a node name (which is not necessarily unique...)
	if (parser.nArgs() != 2) return Messenger::error("A Collect1D node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Collect1D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndCollect1D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!collect1DNodeKeywords().isValid(parser.argc(0))) return collect1DNodeKeywords().errorAndPrintValid(parser.argc(0));
		Collect1DNodeKeyword nk = collect1DNodeKeywords().enumeration(parser.argc(0));
		if (!collect1DNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (Collect1DProcedureNode::EndCollect1DKeyword):
				return true;
			case (Collect1DProcedureNode::QuantityXKeyword):
				// Determine observable from supplied argument
				observable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!observable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect1DNodeKeywords().keyword(nk));
				break;
			case (Collect1DProcedureNode::RangeXKeyword):
				minimum_ = parser.argd(1);
				maximum_ = parser.argd(2);
				binWidth_ = parser.argd(3);
				break;
			case (Collect1DProcedureNode::SubCollectKeyword):
				// Check that a SubCollect branch hasn't already been defined
				if (subCollectBranch_) return Messenger::error("Only one SubCollect branch may be defined in a Collect1D node.\n");

				// Create and parse a new branch
				subCollectBranch_ = new SequenceProcedureNode(scopeStack.currentContext(), "EndSubCollect");
				if (!subCollectBranch_->read(parser, coreData, scopeStack)) return false;
				break;
			case (Collect1DProcedureNode::nCollect1DNodeKeywords):
				return Messenger::error("Unrecognised Collect1D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect1D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool Collect1DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
