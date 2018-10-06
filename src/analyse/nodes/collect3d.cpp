/*
	*** Analysis Node - Collect3D
	*** src/analyse/nodes/collect3d.cpp
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

#include "analyse/nodes/collect3d.h"
#include "analyse/nodecontextstack.h"
#include "analyse/nodes/calculate.h"
#include "math/data3d.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Constructors
AnalysisCollect3DNode::AnalysisCollect3DNode(AnalysisCalculateNode* xObservable, AnalysisCalculateNode* yObservable, AnalysisCalculateNode* zObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin, double zMax, double zBinWidth) : AnalysisNode(AnalysisNode::Collect3DNode)
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
}

AnalysisCollect3DNode::AnalysisCollect3DNode(AnalysisCalculateNode* xyzObservable, double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin, double zMax, double zBinWidth) : AnalysisNode(AnalysisNode::Collect3DNode)
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
AnalysisCollect3DNode::~AnalysisCollect3DNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* Collect3DNodeKeywords[] = { "EndCollect3D", "QuantityXYZ", "QuantityX", "QuantityY", "QuantityZ", "RangeX", "RangeY", "RangeZ" };

// Convert string to node keyword
AnalysisCollect3DNode::Collect3DNodeKeyword AnalysisCollect3DNode::collect3DNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisCollect3DNode::nCollect3DNodeKeywords; ++nk) if (DissolveSys::sameString(s, Collect3DNodeKeywords[nk])) return (AnalysisCollect3DNode::Collect3DNodeKeyword) nk;

	return AnalysisCollect3DNode::nCollect3DNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisCollect3DNode::collect3DNodeKeyword(AnalysisCollect3DNode::Collect3DNodeKeyword nk)
{
	return Collect3DNodeKeywords[nk];
}

/*
 * Data
 */

// Return accumulated data
const Data3D& AnalysisCollect3DNode::accumulatedData() const
{
	if (!histogram_)
	{
		Messenger::error("No histogram pointer set in AnalysisCollect3DNode, so no accumulated data to return.\n");
		static Data3D dummy;
		return dummy;
	}

	return histogram_->accumulatedData();
}


// Return x range minimum
double AnalysisCollect3DNode::xMinimum() const
{
	return xMinimum_;
}

// Return x range maximum
double AnalysisCollect3DNode::xMaximum() const
{
	return xMaximum_;
}

// Return x bin width
double AnalysisCollect3DNode::xBinWidth() const
{
	return xBinWidth_;
}

// Return y range minimum
double AnalysisCollect3DNode::yMinimum() const
{
	return yMinimum_;
}

// Return y range maximum
double AnalysisCollect3DNode::yMaximum() const
{
	return yMaximum_;
}

// Return y bin width
double AnalysisCollect3DNode::yBinWidth() const
{
	return yBinWidth_;
}

// Return z range minimum
double AnalysisCollect3DNode::zMinimum() const
{
	return zMinimum_;
}

// Return z range maximum
double AnalysisCollect3DNode::zMaximum() const
{
	return zMaximum_;
}

// Return z bin width
double AnalysisCollect3DNode::zBinWidth() const
{
	return zBinWidth_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisCollect3DNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
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

	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisCollect3DNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!xObservable_ && (!xyzObservable_))
	{
		Messenger::error("No AnalysisCalculateNode pointer set for X observable in AnalysisCollect3DNode '%s', and no XYZ observable set.\n", name());
		return AnalysisNode::Failure;
	}
	if (!yObservable_ && (!xyzObservable_))
	{
		Messenger::error("No AnalysisCalculateNode pointer set for Y observable in AnalysisCollect3DNode '%s', and no XYZ observable set.\n", name());
		return AnalysisNode::Failure;
	}
	if (!zObservable_ && (!xyzObservable_))
	{
		Messenger::error("No AnalysisCalculateNode pointer set for Z observable in AnalysisCollect3DNode '%s', and no XYZ observable set.\n", name());
		return AnalysisNode::Failure;
	}
#endif
	// Bin the current value of the observable
	if (xyzObservable_) histogram_->bin(xyzObservable_->values());
	else histogram_->bin(xObservable_->value(0), yObservable_->value(0), zObservable_->value(0));

	return AnalysisNode::Success;
}

// Finalise any necessary data after execution
bool AnalysisCollect3DNode::finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	if (!histogram_)
	{
		Messenger::error("No Data3D pointer set in AnalysisCollect3DNode '%s'.\n", name());
		return AnalysisNode::Failure;
	}
#endif
	// Accumulate the current binned data
	histogram_->accumulate();

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisCollect3DNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must also contain a node name (which is not necessarily unique...)
	if (parser.nArgs() != 2) return Messenger::error("A Collect3D node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Add ourselves to the stack
	contextStack.add(this);

	// Read until we encounter the EndCollect3D keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		Collect3DNodeKeyword nk = collect3DNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (Collect3DNodeKeyword::EndCollect3DKeyword):
				return true;
			case (Collect3DNodeKeyword::QuantityXYZKeyword):
				if (xObservable_ || yObservable_ || zObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				xyzObservable_ = contextStack.calculateNodeInScope(parser.argc(1));
				if (!xyzObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeyword(nk));
				break;
			case (Collect3DNodeKeyword::QuantityXKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				xObservable_ = contextStack.calculateNodeInScope(parser.argc(1));
				if (!xObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeyword(nk));
				break;
			case (Collect3DNodeKeyword::QuantityYKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				yObservable_ = contextStack.calculateNodeInScope(parser.argc(1));
				if (!yObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeyword(nk));
				break;
			case (Collect3DNodeKeyword::QuantityZKeyword):
				if (xyzObservable_) return Messenger::error("Can't combine QuantityXYZ with one-dimensional Quantity commands.\n");

				// Determine observable from supplied argument
				zObservable_ = contextStack.calculateNodeInScope(parser.argc(1));
				if (!zObservable_) return Messenger::error("Unrecognised Calculate node '%s' given to %s keyword.\n", parser.argc(1), collect3DNodeKeyword(nk));
				break;
			case (Collect3DNodeKeyword::RangeXKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect3D node keyword '%s' expects exactly three arguments (%i given).\n", collect3DNodeKeyword(nk), parser.nArgs() - 1);
				xMinimum_ = parser.argd(1);
				xMaximum_ = parser.argd(2);
				xBinWidth_ = parser.argd(3);
				break;
			case (Collect3DNodeKeyword::RangeYKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect3D node keyword '%s' expects exactly three arguments (%i given).\n", collect3DNodeKeyword(nk), parser.nArgs() - 1);
				yMinimum_ = parser.argd(1);
				yMaximum_ = parser.argd(2);
				yBinWidth_ = parser.argd(3);
				break;
			case (Collect3DNodeKeyword::RangeZKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect3D node keyword '%s' expects exactly three arguments (%i given).\n", collect3DNodeKeyword(nk), parser.nArgs() - 1);
				zMinimum_ = parser.argd(1);
				zMaximum_ = parser.argd(2);
				zBinWidth_ = parser.argd(3);
				break;
			case (Collect3DNodeKeyword::nCollect3DNodeKeywords):
				return Messenger::error("Unrecognised Collect3D node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect3D node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisCollect3DNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
