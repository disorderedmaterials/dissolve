/*
	*** Analysis Node - Collect
	*** src/analyse/nodes/collect.cpp
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

#include "analyse/nodes/collect.h"
#include "analyse/sitecontextstack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisCollectNode::AnalysisCollectNode() : AnalysisNode()
{
	type_ = AnalysisNode::CollectNode;

	observables_[0] = nObservables;
	observables_[1] = nObservables;
	observables_[2] = nObservables;
	minima_.set(0.0, 0.0, 0.0);
	maxima_.set(10.0, 10.0, 10.0);
	binWidths_.set(0.1, 0.1, 0.1);
}

// Destructor
AnalysisCollectNode::~AnalysisCollectNode()
{
}

/*
 * Observable
 */

// Observable keywords
const char* ObservableKeywords[] = { "Angle", "Distance" };

// Convert string to Observable
AnalysisCollectNode::Observable AnalysisCollectNode::observable(const char* s)
{
	for (int nk=0; nk < AnalysisCollectNode::nObservables; ++nk) if (DissolveSys::sameString(s, ObservableKeywords[nk])) return (AnalysisCollectNode::Observable) nk;

	return AnalysisCollectNode::nObservables;
}

// Convert Observable to string
const char* AnalysisCollectNode::observable(AnalysisCollectNode::Observable obs)
{
	return ObservableKeywords[obs];
}

/*
 * Node Keywords
 */

// Node Keywords (note ordering to allow efficient data setting)
const char* CollectNodeKeywords[] = { "EndCollect", "XAxisLabel", "YAxisLabel", "ZAxisLabel", "XObservable", "YObservable", "ZObservable", "XRange", "YRange", "ZRange" };

// Convert string to node keyword
AnalysisCollectNode::CollectNodeKeyword AnalysisCollectNode::collectNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisCollectNode::nCollectNodeKeywords; ++nk) if (DissolveSys::sameString(s, CollectNodeKeywords[nk])) return (AnalysisCollectNode::CollectNodeKeyword) nk;

	return AnalysisCollectNode::nCollectNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisCollectNode::collectNodeKeyword(AnalysisCollectNode::CollectNodeKeyword nk)
{
	return CollectNodeKeywords[nk];
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AnalysisCollectNode::prepare(Configuration* cfg, const char* dataPrefix, GenericList& targetList)
{
	// Need to check the supplied list for existing data with our name on it. If it's not present, set it up now.
	

	return true;
}

// Execute node, targetting the supplied Configuration
AnalysisNode::NodeExecutionResult AnalysisCollectNode::execute(ProcessPool& procPool, Configuration* cfg, const char* dataPrefix, GenericList& targetList)
{
	Vec3<double> observedValues;

	// Calculate defined observables
	for (int obs = 0; obs < 3; ++obs)
	{
		if (observables_[obs] == AnalysisCollectNode::nObservables) continue;

		switch (observables_[obs])
		{
			case (AnalysisCollectNode::nObservables):
				continue;
				break;
			case (AnalysisCollectNode::DistanceObservable):
				cfg->box()->minimumDistance(sites_[obs][0]->currentSite().origin(), sites_[obs][1]->currentSite().origin());
				break;
			default:
				Messenger::error("Nobody's perfect - the developer has forgotten to add how to collect an observable of type '%s'.\n", observable(observables_[obs]));
				return AnalysisNode::Failure;
		}
	}

	return AnalysisNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AnalysisCollectNode::read(LineParser& parser, SiteContextStack& contextStack)
{
	int index;

	// Read until we encounter the EndCollect keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Is the first argument on the current line a valid control keyword?
		CollectNodeKeyword nk = collectNodeKeyword(parser.argc(0));
		switch (nk)
		{
			case (CollectNodeKeyword::EndCollectKeyword):
				return true;
			case (CollectNodeKeyword::XLabelKeyword):
			case (CollectNodeKeyword::YLabelKeyword):
			case (CollectNodeKeyword::ZLabelKeyword):
				axisLabels_[nk-CollectNodeKeyword::XObservableKeyword] = parser.argc(1);
				break;
			case (CollectNodeKeyword::XObservableKeyword):
			case (CollectNodeKeyword::YObservableKeyword):
			case (CollectNodeKeyword::ZObservableKeyword):
				// Determine observable from supplied argument
				observables_[nk-CollectNodeKeyword::XObservableKeyword] = observable(parser.argc(1));
				if (observables_[nk-CollectNodeKeyword::XObservableKeyword] == AnalysisCollectNode::nObservables) return Messenger::error("Unrecognised observable '%s' given to %s keyword.\n", parser.argc(1), collectNodeKeyword(nk));

				// Read node (site) names
				for (int n=2; n<parser.nArgs(); ++n)
				{
					// Each argument should be a named site on the stack, so find it...
					if (!contextStack.hasSite(parser.argc(n))) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(n), collectNodeKeyword(CollectNodeKeyword::XObservableKeyword));
					else sites_[nk-CollectNodeKeyword::XObservableKeyword].add(contextStack.siteNode(parser.argc(n)));
				}
				break;
			case (CollectNodeKeyword::XRangeKeyword):
			case (CollectNodeKeyword::YRangeKeyword):
			case (CollectNodeKeyword::ZRangeKeyword):
				// Check that we have the right number of arguments first...
				if (parser.nArgs() != 4) return Messenger::error("Collect node keyword '%s' expects exactly three arguments (%i given).\n", collectNodeKeyword(CollectNodeKeyword::XObservableKeyword), parser.nArgs() - 1);
				minima_.set(nk-CollectNodeKeyword::XObservableKeyword, parser.argd(1));
				maxima_.set(nk-CollectNodeKeyword::XObservableKeyword, parser.argd(2));
				binWidths_.set(nk-CollectNodeKeyword::XObservableKeyword, parser.argd(3));
				break;
			case (CollectNodeKeyword::nCollectNodeKeywords):
				return Messenger::error("Unrecognised Collect node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Collect node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AnalysisCollectNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
