/*
	*** Analysis Node - Collect2D
	*** src/analyse/nodes/collect2d.cpp
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

#include "analyse/nodes/collect2d.h"
#include "analyse/nodecontextstack.h"
#include "analyse/nodes/calculate.h"
#include "analyse/nodes/sequence.h"
#include "math/data2d.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Constructor
AnalysisCollect2DNode::AnalysisCollect2DNode(AnalysisCalculateNode* xObservable, AnalysisCalculateNode* yObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth) : AnalysisNode(AnalysisNode::Collect2DNode)
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
AnalysisCollect2DNode::~AnalysisCollect2DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* Collect2DNodeKeywords[] = { "EndCollect2D", "QuantityX", "QuantityY", "RangeX", "RangeY", "SubCollect" };

// Convert string to node keyword
AnalysisCollect2DNode::Collect2DNodeKeyword AnalysisCollect2DNode::collect2DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisCollect2DNode::nCollect2DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Collect2DNodeKeywords[nk])) return (AnalysisCollect2DNode::Collect2DNodeKeyword) nk;

	return AnalysisCollect2DNode::nCollect2DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisCollect2DNode::collect2DNodeKeyword(AnalysisCollect2DNode::Collect2DNodeKeyword nk)
{
	return Collect2DNodeKeywords[nk];
}

/*
 * Data
 */

// Return accumulated data
const Data2D& AnalysisCollect2DNode::accumulatedData() const
{
	if (!histogram_)
	{
		Messenger::error("No histogram pointer set in AnalysisCollect2DNode, so no accumulated data to return.\n");
		static Data2D dummy;
		return dummy;
	}

	return histogram_->accumulatedData();
}

// Return x range minimum
double AnalysisCollect2DNode::xMinimum() const
{
	return xMinimum_;
}

// Return x range maximum
double AnalysisCollect2DNode::xMaximum() const
{
	return xMaximum_;
}

// Return x bin width
double AnalysisCollect2DNode::xBinWidth() const
{
	return xBinWidth_;
}

// Return y range minimum
double AnalysisCollect2DNode::yMinimum() const
{
	return yMinimum_;
}

// Return y range maximum
double AnalysisCollect2DNode::yMaximum() const
{
	return yMaximum_;
}

// Return y bin width
double AnalysisCollect2DNode::yBinWidth() const
{
	return yBinWidth_;
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
AnalysisSequenceNode* AnalysisCollect2DNode::addSubCollectBranch()
{
	if (!subCollectBranch_) subCollectBranch_ = new AnalysisSequenceNode();

	subCollectBranch_->setParent(parent());

	return subCollectBranch_;
}

// Add specified node to subcollection sequence
void AnalysisCollect2DNode::addToSubCollectBranch(AnalysisNode* node)
{
	addSubCollectBranch()->addNode(node);
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisCollect2DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
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
AnalysisNode::NodeExecutionResult AnalysisCollect2DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!xObservable_)
	{
		Messenger::error("No AnalysisCalculateNode pointer set for X observable in AnalysisCollect2DNode '%s'.\n", name());
		return AnalysisNode::Failure;
	}
	if (!yObservable_)
	{
		Messenger::error("No AnalysisCalculateNode pointer set for Y observable in AnalysisCollect2DNode '%s'.\n", name());
		return AnalysisNode::Failure;
	}
#endif
	// Bin the current value of the observable
	if (histogram_->bin(xObservable_->value(0), yObservable_->value(0)) && subCollectBranch_) return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisCollect2DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!histogram_)
	{
		Messenger::error("No Data2D pointer set in AnalysisCollect2DNode '%s'.\n", name());
		return AnalysisNode::Failure;
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
bool AnalysisCollect2DNode::read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain a node name (which is not necessarily unique...)
	if (parser.nArgs() != 2) return Messenger::error("A Collect2D node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Collect2D node '%s' to context stack.\n", name());

	// Read until we encounter the EndCollect2D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Collect2DNodeKeyword nk = collect2DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (AnalysisCollect2DNode::EndCollect2DKeyword):
				return true;
			case (AnalysisCollect2DNode::QuantityXKeyword):
				// Determine observable from supplied argument
				xObservable_ = dynamic_cast<AnalysisCalculateNode*>(contextStack.nodeInScope(parser.argc(1), AnalysisNode::CalculateNode));
				if (!xObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect2DNodeKeyword(nk));
				break;
			case (AnalysisCollect2DNode::QuantityYKeyword):
				// Determine observable from supplied argument
				yObservable_ = dynamic_cast<AnalysisCalculateNode*>(contextStack.nodeInScope(parser.argc(1), AnalysisNode::CalculateNode));
				if (!yObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect2DNodeKeyword(nk));
				break;
			case (AnalysisCollect2DNode::RangeXKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect2D node keyword '%s' expects exactly three arguments (%i given).\n", collect2DNodeKeyword(AnalysisCollect2DNode::RangeXKeyword), parser.nArgs() - 1);
				xMinimum_ = parser.argd(1);
				xMaximum_ = parser.argd(2);
				xBinWidth_ = parser.argd(3);
				break;
			case (AnalysisCollect2DNode::RangeYKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect2D node keyword '%s' expects exactly three arguments (%i given).\n", collect2DNodeKeyword(AnalysisCollect2DNode::RangeXKeyword), parser.nArgs() - 1);
				yMinimum_ = parser.argd(1);
				yMaximum_ = parser.argd(2);
				yBinWidth_ = parser.argd(3);
				break;
			case (AnalysisCollect2DNode::SubCollectKeyword):
				// Check that a SubCollect branch hasn't already been defined
				if (subCollectBranch_) return Messenger::error("Only one SubCollect branch may be defined in a Collect2D node.\n");

				// Create and parse a new branch
				subCollectBranch_ = new AnalysisSequenceNode("EndSubCollect");
				subCollectBranch_->setParent(parent());
				if (!subCollectBranch_->read(parser, coreData, contextStack)) return false;
				break;
			case (AnalysisCollect2DNode::nCollect2DNodeKeywords):
				return Messenger::error("Unrecognised Collect2D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect2D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisCollect2DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
