/*
	*** Procedure Node - Collect2D
	*** src/procedure/nodes/collect2d.cpp
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

#include "procedure/nodes/collect2d.h"
#include "procedure/nodescopestack.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/sequence.h"
#include "math/data2d.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
Collect2DProcedureNode::Collect2DProcedureNode(CalculateProcedureNode* xObservable, CalculateProcedureNode* yObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth) : ProcedureNode(ProcedureNode::Collect2DNode)
{
	xObservable_ = xObservable;
	yObservable_ = yObservable;
	xMinimum_ = xMin;
	xMaximum_ = xMax;
	xBinWidth_ = xBinWidth;
	yMinimum_ = yMin;
	yMaximum_ = yMax;
	yBinWidth_ = yBinWidth;

	// Branches
	subCollectBranch_ = NULL;
}

// Destructor
Collect2DProcedureNode::~Collect2DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified usage type is allowed for this node
bool Collect2DProcedureNode::isUsageTypeAllowed(ProcedureNode::NodeUsageType usageType)
{
	return (usageType == ProcedureNode::AnalysisUsageType);
}

/*
 * Node Keywords
 */

// Return enum option info for Collect2DNodeKeyword
EnumOptions<Collect2DProcedureNode::Collect2DNodeKeyword> Collect2DProcedureNode::collect2DNodeKeywords()
{
	static EnumOptionsList Collect2DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Collect2DProcedureNode::EndCollect2DKeyword,		"EndCollect2D") <<
		EnumOption(Collect2DProcedureNode::QuantityXKeyword,		"QuantityX") <<
		EnumOption(Collect2DProcedureNode::QuantityYKeyword,		"QuantityY") <<
		EnumOption(Collect2DProcedureNode::RangeXKeyword,		"RangeX") <<
		EnumOption(Collect2DProcedureNode::RangeYKeyword,		"RangeY") <<
		EnumOption(Collect2DProcedureNode::SubCollectKeyword,		"SubCollect");

	static EnumOptions<Collect2DProcedureNode::Collect2DNodeKeyword> options("Collect2DNodeKeyword", Collect2DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Return accumulated data
const Data2D& Collect2DProcedureNode::accumulatedData() const
{
	if (!histogram_)
	{
		Messenger::error("No histogram pointer set in Collect2DProcedureNode, so no accumulated data to return.\n");
		static Data2D dummy;
		return dummy;
	}

	return histogram_->accumulatedData();
}

// Return x range minimum
double Collect2DProcedureNode::xMinimum() const
{
	return xMinimum_;
}

// Return x range maximum
double Collect2DProcedureNode::xMaximum() const
{
	return xMaximum_;
}

// Return x bin width
double Collect2DProcedureNode::xBinWidth() const
{
	return xBinWidth_;
}

// Return y range minimum
double Collect2DProcedureNode::yMinimum() const
{
	return yMinimum_;
}

// Return y range maximum
double Collect2DProcedureNode::yMaximum() const
{
	return yMaximum_;
}

// Return y bin width
double Collect2DProcedureNode::yBinWidth() const
{
	return yBinWidth_;
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode* Collect2DProcedureNode::addSubCollectBranch()
{
	if (!subCollectBranch_) subCollectBranch_ = new SequenceProcedureNode();

	// Pass on our Procedure parent
	subCollectBranch_->setProcedure(procedure());

	return subCollectBranch_;
}

// Add specified node to subcollection sequence
void Collect2DProcedureNode::addToSubCollectBranch(ProcedureNode* node)
{
	addSubCollectBranch()->addNode(node);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect2DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Construct our data name, and search for it in the supplied list
	CharString dataName("%s_%s_Bins", name(), cfg->niceName());
	bool created;
	Histogram2D& target = GenericListHelper<Histogram2D>::realise(targetList, dataName.get(), prefix, GenericItem::InRestartFileFlag, &created);
	if (created)
	{
		Messenger::printVerbose("Two-dimensional histogram data for '%s' was not in the target list, so it will now be initialised...\n", name());
		target.initialise(xMinimum_, xMaximum_, xBinWidth_, yMinimum_, yMaximum_, yBinWidth_);
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
ProcedureNode::NodeExecutionResult Collect2DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!xObservable_)
	{
		Messenger::error("No CalculateProcedureNode pointer set for X observable in Collect2DProcedureNode '%s'.\n", name());
		return ProcedureNode::Failure;
	}
	if (!yObservable_)
	{
		Messenger::error("No CalculateProcedureNode pointer set for Y observable in Collect2DProcedureNode '%s'.\n", name());
		return ProcedureNode::Failure;
	}
#endif
	// Bin the current value of the observable
	if (histogram_->bin(xObservable_->value(0), yObservable_->value(0)) && subCollectBranch_) return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect2DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!histogram_)
	{
		Messenger::error("No Data2D pointer set in Collect2DProcedureNode '%s'.\n", name());
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
bool Collect2DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser must also contain a node name (which is not necessarily unique...)
	if (parser.nArgs() != 2) return Messenger::error("A Collect2D node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Collect2D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndCollect2D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Collect2DNodeKeyword nk = collect2DNodeKeywords().enumeration(parser.argc(0));
		switch (nk)
		{
			case (Collect2DProcedureNode::EndCollect2DKeyword):
				return true;
			case (Collect2DProcedureNode::QuantityXKeyword):
				// Determine observable from supplied argument
				xObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!xObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect2DNodeKeywords().keyword(nk));
				break;
			case (Collect2DProcedureNode::QuantityYKeyword):
				// Determine observable from supplied argument
				yObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!yObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect2DNodeKeywords().keyword(nk));
				break;
			case (Collect2DProcedureNode::RangeXKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect2D node keyword '%s' expects exactly three arguments (%i given).\n", collect2DNodeKeywords().keyword(Collect2DProcedureNode::RangeXKeyword), parser.nArgs() - 1);
				xMinimum_ = parser.argd(1);
				xMaximum_ = parser.argd(2);
				xBinWidth_ = parser.argd(3);
				break;
			case (Collect2DProcedureNode::RangeYKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect2D node keyword '%s' expects exactly three arguments (%i given).\n", collect2DNodeKeywords().keyword(Collect2DProcedureNode::RangeXKeyword), parser.nArgs() - 1);
				yMinimum_ = parser.argd(1);
				yMaximum_ = parser.argd(2);
				yBinWidth_ = parser.argd(3);
				break;
			case (Collect2DProcedureNode::SubCollectKeyword):
				// Check that a SubCollect branch hasn't already been defined
				if (subCollectBranch_) return Messenger::error("Only one SubCollect branch may be defined in a Collect2D node.\n");

				// Create and parse a new branch
				subCollectBranch_ = new SequenceProcedureNode("EndSubCollect");
				subCollectBranch_->setProcedure(procedure());
				if (!subCollectBranch_->read(parser, coreData, scopeStack)) return false;
				break;
			case (Collect2DProcedureNode::nCollect2DNodeKeywords):
				return Messenger::error("Unrecognised Collect2D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect2D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool Collect2DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
