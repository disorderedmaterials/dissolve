/*
	*** Analysis Node - Calculate
	*** src/analyse/nodes/calculate.cpp
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

#include "analyse/nodes/calculate.h"
#include "analyse/nodes/select.h"
#include "analyse/nodecontextstack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisCalculateNode::AnalysisCalculateNode(AnalysisCalculateNode::Observable observable, AnalysisSelectNode* site0, AnalysisSelectNode* site1, AnalysisSelectNode* site2, AnalysisSelectNode* site3) : AnalysisNode(AnalysisNode::CalculateNode)
{
	observable_ = observable;
	sites_[0] = site0;
	sites_[1] = site1;
	sites_[2] = site2;
	sites_[3] = site3;
}

// Destructor
AnalysisCalculateNode::~AnalysisCalculateNode()
{
}

/*
 * Node Keywords
 */

// Node Keywords
const char* CalculateNodeKeywords[] = { "Angle", "Distance", "EndCalculate" };

// Convert string to node keyword
AnalysisCalculateNode::CalculateNodeKeyword AnalysisCalculateNode::calculateNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisCalculateNode::nCalculateNodeKeywords; ++nk) if (DissolveSys::sameString(s, CalculateNodeKeywords[nk])) return (AnalysisCalculateNode::CalculateNodeKeyword) nk;

	return AnalysisCalculateNode::nCalculateNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisCalculateNode::calculateNodeKeyword(AnalysisCalculateNode::CalculateNodeKeyword nk)
{
	return CalculateNodeKeywords[nk];
}

/*
 * Observables
 */

// Observable keywords
const char* ObservableKeywords[] = { "Angle", "Distance" };
int ObservableNSites[] = { 3, 2 };

// Convert string to Observable
AnalysisCalculateNode::Observable AnalysisCalculateNode::observable(const char* s)
{
	for (int nk=0; nk < AnalysisCalculateNode::nObservables; ++nk) if (DissolveSys::sameString(s, ObservableKeywords[nk])) return (AnalysisCalculateNode::Observable) nk;

	return AnalysisCalculateNode::nObservables;
}

// Convert Observable to string
const char* AnalysisCalculateNode::observable(AnalysisCalculateNode::Observable obs)
{
	return ObservableKeywords[obs];
}

// Number of sites required to calculate Observable
int AnalysisCalculateNode::observableNSites(AnalysisCalculateNode::Observable obs)
{
	return ObservableNSites[obs];
}

/*
 * Observable Target
 */

// Return last calculated value of observable
double AnalysisCalculateNode::value() const
{
	return value_;
}

/* 
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisCalculateNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisCalculateNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	for (int n=0; n<observableNSites(observable_); ++n)
	{
		if (!sites_[n])
		{
			Messenger::error("Observable site %i is not set.\n", n);
			return AnalysisNode::Failure;
		}
		if (sites_[n]->currentSite() == NULL)
		{
			Messenger::error("Observable %i has no current site.\n", n);
			return AnalysisNode::Failure;
		}
	}
#endif
	// Determine the value of the requested observable
	switch (observable_)
	{
		case (Observable::AngleObservable):
			value_ = cfg->box()->angleInDegrees(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin(), sites_[2]->currentSite()->origin());
			break;
		case (Observable::DistanceObservable):
			value_ = cfg->box()->minimumDistance(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());
			break;
		default:
			Messenger::error("Mr Developer Man has not implemented calculation of this observable quantity (%i).\n", observable_);
			return AnalysisNode::Failure;
	}

	return AnalysisNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisCalculateNode::read(LineParser& parser, NodeContextStack& contextStack)
{
	// The current line in the parser must contain a label (name) for the node, and which must not currently exist on the context stack
	if (parser.nArgs() != 2) return Messenger::error("A Calculate node must be given a suitable name.\n");
	if (contextStack.calculateNodeInScope(parser.argc(1))) return Messenger::error("A Calculate node named '%s' is already in scope, and cannot be redefined.\n", parser.argc(1));
	setName(parser.argc(1));

	// Add ourselves to the context stack
	if (!contextStack.add(this)) return Messenger::error("Error adding Select node '%s' to context stack.\n", name());

	// Read until we encounter the EndCalculate keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		CalculateNodeKeyword nk = calculateNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (CalculateNodeKeyword::AngleKeyword):
				if (parser.nArgs() != 4) return Messenger::error("The %s keyword expects exactly three arguments.\n", calculateNodeKeyword(CalculateNodeKeyword::AngleKeyword));

				observable_ = Observable::AngleObservable;

				// First Site argument (point 'i' in angle i-j-k)
				sites_[0] = contextStack.selectNodeInScope(parser.argc(1));
				if (!sites_[0]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(1), calculateNodeKeyword(CalculateNodeKeyword::AngleKeyword));

				// Second Site argument (point 'j' in angle i-j-k)
				sites_[1] = contextStack.selectNodeInScope(parser.argc(2));
				if (!sites_[1]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(2), calculateNodeKeyword(CalculateNodeKeyword::AngleKeyword));

				// Third Site argument (point 'k' in angle i-j-k)
				sites_[2] = contextStack.selectNodeInScope(parser.argc(3));
				if (!sites_[2]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(3), calculateNodeKeyword(CalculateNodeKeyword::AngleKeyword));
				break;
			case (CalculateNodeKeyword::DistanceKeyword):
				if (parser.nArgs() != 3) return Messenger::error("The %s keyword expects exactly two arguments.\n", calculateNodeKeyword(CalculateNodeKeyword::DistanceKeyword));

				observable_ = Observable::DistanceObservable;

				// First Site argument (point 'i' in distance i-j)
				sites_[0] = contextStack.selectNodeInScope(parser.argc(1));
				if (!sites_[0]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(1), calculateNodeKeyword(CalculateNodeKeyword::DistanceKeyword));

				// Second Site argument (point 'j' in distance i-j)
				sites_[1] = contextStack.selectNodeInScope(parser.argc(2));
				if (!sites_[1]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(2), calculateNodeKeyword(CalculateNodeKeyword::DistanceKeyword));
				else 
				break;
			case (CalculateNodeKeyword::EndCalculateKeyword):
				return true;
			case (CalculateNodeKeyword::nCalculateNodeKeywords):
				return Messenger::error("Unrecognised Calculate node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Calculate node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisCalculateNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
