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
