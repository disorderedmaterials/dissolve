/*
	*** Procedure Node - Collect3D
	*** src/procedure/nodes/collect3d.cpp
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

#include "procedure/nodes/collect3d.h"
#include "procedure/nodescopestack.h"
#include "procedure/nodes/calculate.h"
#include "procedure/nodes/sequence.h"
#include "math/data3d.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructors
Collect3DProcedureNode::Collect3DProcedureNode(CalculateProcedureNode* xObservable, CalculateProcedureNode* yObservable, CalculateProcedureNode* zObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin, double zMax, double zBinWidth) : ProcedureNode(ProcedureNode::Collect3DNode)
{
	xyzObservable_ = NULL;

	xObservable_ = xObservable;
	xMinimum_ = xMin;
	xMaximum_ = xMax;
	xBinWidth_ = xBinWidth;

	yObservable_ = yObservable;
	yMinimum_ = yMin;
	yMaximum_ = yMax;
	yBinWidth_ = yBinWidth;

	zObservable_ = zObservable;
	zMinimum_ = zMin;
	zMaximum_ = zMax;
	zBinWidth_ = zBinWidth;

	// Branches
	subCollectBranch_ = NULL;
}

Collect3DProcedureNode::Collect3DProcedureNode(CalculateProcedureNode* xyzObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin, double zMax, double zBinWidth) : ProcedureNode(ProcedureNode::Collect3DNode)
{
	xObservable_ = NULL;
	yObservable_ = NULL;
	zObservable_ = NULL;

	xyzObservable_ = xyzObservable;

	xMinimum_ = xMin;
	xMaximum_ = xMax;
	xBinWidth_ = xBinWidth;

	yMinimum_ = yMin;
	yMaximum_ = yMax;
	yBinWidth_ = yBinWidth;

	zMinimum_ = zMin;
	zMaximum_ = zMax;
	zBinWidth_ = zBinWidth;
}

// Destructor
Collect3DProcedureNode::~Collect3DProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect3DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Collect3DNodeKeyword
EnumOptions<Collect3DProcedureNode::Collect3DNodeKeyword> Collect3DProcedureNode::collect3DNodeKeywords()
{
	static EnumOptionsList Collect3DNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(Collect3DProcedureNode::EndCollect3DKeyword,		"EndCollect3D") <<
		EnumOption(Collect3DProcedureNode::QuantityXYZKeyword,		"QuantityXYZ",	1) <<
		EnumOption(Collect3DProcedureNode::QuantityXKeyword,		"QuantityX",	1) <<
		EnumOption(Collect3DProcedureNode::QuantityYKeyword,		"QuantityY",	1) <<
		EnumOption(Collect3DProcedureNode::QuantityZKeyword,		"QuantityZ",	1) <<
		EnumOption(Collect3DProcedureNode::RangeXKeyword,		"RangeX",	3) <<
		EnumOption(Collect3DProcedureNode::RangeYKeyword,		"RangeY",	3) <<
		EnumOption(Collect3DProcedureNode::RangeZKeyword,		"RangeZ",	3) <<
		EnumOption(Collect3DProcedureNode::SubCollectKeyword,		"SubCollect");

	static EnumOptions<Collect3DProcedureNode::Collect3DNodeKeyword> options("Collect3DNodeKeyword", Collect3DNodeTypeKeywords);

	return options;
}

/*
 * Data
 */

// Return accumulated data
const Data3D& Collect3DProcedureNode::accumulatedData() const
{
	if (!histogram_)
	{
		Messenger::error("No histogram pointer set in Collect3DProcedureNode, so no accumulated data to return.\n");
		static Data3D dummy;
		return dummy;
	}

	return histogram_->accumulatedData();
}


// Return x range minimum
double Collect3DProcedureNode::xMinimum() const
{
	return xMinimum_;
}

// Return x range maximum
double Collect3DProcedureNode::xMaximum() const
{
	return xMaximum_;
}

// Return x bin width
double Collect3DProcedureNode::xBinWidth() const
{
	return xBinWidth_;
}

// Return y range minimum
double Collect3DProcedureNode::yMinimum() const
{
	return yMinimum_;
}

// Return y range maximum
double Collect3DProcedureNode::yMaximum() const
{
	return yMaximum_;
}

// Return y bin width
double Collect3DProcedureNode::yBinWidth() const
{
	return yBinWidth_;
}

// Return z range minimum
double Collect3DProcedureNode::zMinimum() const
{
	return zMinimum_;
}

// Return z range maximum
double Collect3DProcedureNode::zMaximum() const
{
	return zMaximum_;
}

// Return z bin width
double Collect3DProcedureNode::zBinWidth() const
{
	return zBinWidth_;
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode* Collect3DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
	if (!subCollectBranch_) subCollectBranch_ = new SequenceProcedureNode(context);

	return subCollectBranch_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect3DProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Construct our data name, and search for it in the supplied list
	CharString dataName("%s_%s_Bins", name(), cfg->niceName());
	bool created;
	Histogram3D& target = GenericListHelper<Histogram3D>::realise(targetList, dataName.get(), prefix, GenericItem::InRestartFileFlag, &created);
	if (created)
	{
		Messenger::printVerbose("One-dimensional histogram data for '%s' was not in the target list, so it will now be initialised...\n", name());
		target.initialise(xMinimum_, xMaximum_, xBinWidth_, yMinimum_, yMaximum_, yBinWidth_, zMinimum_, zMaximum_, zBinWidth_);
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
ProcedureNode::NodeExecutionResult Collect3DProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!xObservable_ && (!xyzObservable_))
	{
		Messenger::error("No CalculateProcedureNode pointer set for X observable in Collect3DProcedureNode '%s', and no XYZ observable set.\n", name());
		return ProcedureNode::Failure;
	}
	if (!yObservable_ && (!xyzObservable_))
	{
		Messenger::error("No CalculateProcedureNode pointer set for Y observable in Collect3DProcedureNode '%s', and no XYZ observable set.\n", name());
		return ProcedureNode::Failure;
	}
	if (!zObservable_ && (!xyzObservable_))
	{
		Messenger::error("No CalculateProcedureNode pointer set for Z observable in Collect3DProcedureNode '%s', and no XYZ observable set.\n", name());
		return ProcedureNode::Failure;
	}
#endif
	// Bin the current value of the observable
	if (xyzObservable_)
	{
		if (histogram_->bin(xyzObservable_->values()) && subCollectBranch_) return subCollectBranch_->execute(procPool, cfg, prefix, targetList);
	}
	else
	{
		if (histogram_->bin(xObservable_->value(0), yObservable_->value(0), zObservable_->value(0)) && subCollectBranch_) return subCollectBranch_->execute(procPool, cfg, prefix, targetList);
	}

	return ProcedureNode::Success;
}

// Finalise any necessary data after execution
bool Collect3DProcedureNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!histogram_)
	{
		Messenger::error("No Data3D pointer set in Collect3DProcedureNode '%s'.\n", name());
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
bool Collect3DProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// The current line in the parser must also contain a node name (which is not necessarily unique...)
	if (parser.nArgs() != 2) return Messenger::error("A Collect3D node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Add ourselves to the scope stack
	if (!scopeStack.add(this)) return Messenger::error("Error adding Collect3D node '%s' to scope stack.\n", name());

	// Read until we encounter the EndCollect3D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!collect3DNodeKeywords().isValid(parser.argc(0))) return collect3DNodeKeywords().errorAndPrintValid(parser.argc(0));
		Collect3DNodeKeyword nk = collect3DNodeKeywords().enumeration(parser.argc(0));
		if (!collect3DNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (Collect3DProcedureNode::EndCollect3DKeyword):
				return true;
			case (Collect3DProcedureNode::QuantityXYZKeyword):
				if (xObservable_ || yObservable_ || zObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				xyzObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!xyzObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeywords().keyword(nk));
				break;
			case (Collect3DProcedureNode::QuantityXKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				xObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!xObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeywords().keyword(nk));
				break;
			case (Collect3DProcedureNode::QuantityYKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				yObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!yObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeywords().keyword(nk));
				break;
			case (Collect3DProcedureNode::QuantityZKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				zObservable_ = dynamic_cast<CalculateProcedureNode*>(scopeStack.nodeInScope(parser.argc(1), ProcedureNode::CalculateNode));
				if (!zObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeywords().keyword(nk));
				break;
			case (Collect3DProcedureNode::RangeXKeyword):
				xMinimum_ = parser.argd(1);
				xMaximum_ = parser.argd(2);
				xBinWidth_ = parser.argd(3);
				break;
			case (Collect3DProcedureNode::RangeYKeyword):
				yMinimum_ = parser.argd(1);
				yMaximum_ = parser.argd(2);
				yBinWidth_ = parser.argd(3);
				break;
			case (Collect3DProcedureNode::RangeZKeyword):
				zMinimum_ = parser.argd(1);
				zMaximum_ = parser.argd(2);
				zBinWidth_ = parser.argd(3);
				break;
			case (Collect3DProcedureNode::SubCollectKeyword):
				// Check that a SubCollect branch hasn't already been defined
				if (subCollectBranch_) return Messenger::error("Only one SubCollect branch may be defined in a Collect3D node.\n");

				// Create and parse a new branch
				subCollectBranch_ = new SequenceProcedureNode(scopeStack.currentContext(), "EndSubCollect");
				if (!subCollectBranch_->read(parser, coreData, scopeStack)) return false;
				break;
			case (Collect3DProcedureNode::nCollect3DNodeKeywords):
				return Messenger::error("Unrecognised Collect3D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect3D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool Collect3DProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
