/*
	*** Analysis Node - Select
	*** src/analyse/nodes/select.cpp
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

#include "analyse/nodes/select.h"
#include "analyse/nodes/sequence.h"
#include "analyse/nodecontextstack.h"
#include "analyse/sitereference.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AnalysisSelectBaseNode::AnalysisSelectBaseNode()
{
	sameMolecule_= NULL;
	forEachBranch_ = NULL;
	currentSiteIndex_ = -1;
	nCumulativeSites_ = 0;
	nSelections_ = 0;
}

// Destructor
AnalysisSelectBaseNode::~AnalysisSelectBaseNode()
{
	if (forEachBranch_) delete forEachBranch_;
}

/*
 * Node Keywords
 */

// Node Keywords
const char* SelectBaseNodeKeywords[] = { "ExcludeSameMolecule", "ExcludeSameSite", "ForEach", "SameMoleculeAsSite", "Site" };

// Convert string to node keyword
AnalysisSelectBaseNode::SelectBaseNodeKeyword AnalysisSelectBaseNode::selectBaseNodeKeyword(const char* s)
{
	for (int nk=0; nk < AnalysisSelectBaseNode::nSelectBaseNodeKeywords; ++nk) if (DissolveSys::sameString(s, SelectBaseNodeKeywords[nk])) return (AnalysisSelectBaseNode::SelectBaseNodeKeyword) nk;

	return AnalysisSelectBaseNode::nSelectBaseNodeKeywords;
}

// Convert node keyword to string
const char* AnalysisSelectBaseNode::selectBaseNodeKeyword(AnalysisSelectBaseNode::SelectBaseNodeKeyword nk)
{
	return SelectBaseNodeKeywords[nk];
}

// Parse current keyword (if recognised) returning 1 on success, 0 on recognised but failed, and -1 for not recognised
int AnalysisSelectBaseNode::parseBaseKeyword(LineParser& parser, NodeContextStack& contextStack)
{
	// Is the first argument on the current line a valid control keyword?
	SelectBaseNodeKeyword nk = selectBaseNodeKeyword(parser.argc(0));
	switch (nk)
	{
		case (SelectBaseNodeKeyword::ExcludeSameMoleculeKeyword):
			for (int n=1; n<parser.nArgs(); ++n)
			{
				AnalysisSelectBaseNode* otherNode = contextStack.selectNodeInScope(parser.argc(n));
				if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectBaseNodeKeyword(nk));
				if (!addSameMoleculeExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectBaseNodeKeyword(nk));
			}
			break;
		case (SelectBaseNodeKeyword::ExcludeSameSiteKeyword):
			for (int n=1; n<parser.nArgs(); ++n)
			{
				AnalysisSelectBaseNode* otherNode = contextStack.selectNodeInScope(parser.argc(n));
				if (!otherNode) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(n), selectBaseNodeKeyword(nk));
				if (!addSameSiteExclusion(otherNode)) return Messenger::error("Duplicate site given to %s keyword.\n", selectBaseNodeKeyword(nk));
			}
			break;
		case (SelectBaseNodeKeyword::ForEachKeyword):
			// Check that a ForEach branch hasn't already been defined
			if (forEachBranch_) return Messenger::error("Only one ForEach branch may be defined in a selection node.\n");

			// Create and parse a new branch
			forEachBranch_ = new AnalysisSequenceNode("EndForEach");
			if (!forEachBranch_->read(parser, contextStack)) return false;
			break;
		case (SelectBaseNodeKeyword::SameMoleculeAsSiteKeyword):
			if (sameMolecule_) return Messenger::error("Same molecule restriction has already been set, and cannot be set again.\n");
			sameMolecule_ = contextStack.selectNodeInScope(parser.argc(1));
			if (!sameMolecule_) return Messenger::error("Unrecognised selection node '%s' given to %s keyword.\n", parser.argc(1), selectBaseNodeKeyword(nk));
			break;
		case (SelectBaseNodeKeyword::nSelectBaseNodeKeywords):
			return -1;
			break;
		default:
			return Messenger::error("Epic Developer Fail - Don't know how to deal with the Select node keyword '%s'.\n", parser.argc(0));
	}

	return 1;
}

/*
 * Data
 */

// Add "same molecule" exclusion
bool AnalysisSelectBaseNode::addSameMoleculeExclusion(AnalysisSelectBaseNode* node)
{
	if (sameMoleculeExclusions_.contains(node)) return false;
	else sameMoleculeExclusions_.add(node);

	return true;
}

// Add "same site" exclusion
bool AnalysisSelectBaseNode::addSameSiteExclusion(AnalysisSelectBaseNode* node)
{
	if (sameSiteExclusions_.contains(node)) return false;
	else sameSiteExclusions_.add(node);

	return true;
}

// Return Molecule (from site) in which the site must exist
const Molecule* AnalysisSelectBaseNode::sameMoleculeMolecule()
{
	if (!sameMolecule_)
	{
		Messenger::warn("Requested Molecule from AnalysisSelectBaseNode::sameMolecule_, but no selection node is defined for it.\n");
		return NULL;
	}

	const Site* site = sameMolecule_->currentSite();
	if (!site)
	{
		Messenger::warn("Requested Molecule from AnalysisSelectBaseNode::sameMolecule_, but there is no current site.\n");
		return NULL;
	}

	return site->molecule();
}

// Add and return ForEach sequence
AnalysisSequenceNode* AnalysisSelectBaseNode::addForEachBranch()
{
	if (!forEachBranch_) forEachBranch_ = new AnalysisSequenceNode();

	return forEachBranch_;
}

// Add specified node to ForEach sequence
void AnalysisSelectBaseNode::addToForEachBranch(AnalysisNode* node)
{
	addForEachBranch()->addNode(node);
}

/*
 * Site Information
 */

// Return the number of available sites in the current stack, if any
int AnalysisSelectBaseNode::nSitesInStack() const
{
	return sites_.nItems();
}

// Return the average number of sites selected
double AnalysisSelectBaseNode::nAverageSites() const
{
	return double(nCumulativeSites_) / nSelections_;
}

// Return the cumulative number of sites ever selected
int AnalysisSelectBaseNode::nCumulativeSites() const
{
	return nCumulativeSites_;
}

// Return current site
const Site* AnalysisSelectBaseNode::currentSite() const
{
	return (currentSiteIndex_ == -1 ? NULL : sites_.constAt(currentSiteIndex_));
}
