/*
	*** Analysis Node - Select Base
	*** src/analyse/nodes/select.h
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

#ifndef DISSOLVE_ANALYSISSELECTBASE_H
#define DISSOLVE_ANALYSISSELECTBASE_H

#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisSequenceNode;
class SiteStack;
class Species;
class SpeciesSite;

// Select Node
class AnalysisSelectBaseNode
{
	public:
	// Constructor
	AnalysisSelectBaseNode();
	// Destructor
	~AnalysisSelectBaseNode();


	/*
	 * General Data
	 */
	protected:
	// List of other sites (nodes) which will exclude one of our sites if it has the same Molecule parent
	RefList<AnalysisSelectBaseNode,bool> sameMoleculeExclusions_;
	// List of other sites (nodes) which will exclude one of our sites if it is the same site
	RefList<AnalysisSelectBaseNode,bool> sameSiteExclusions_;
	// Array containing pointers to our selected sites
	Array<const Site*> sites_;
	// Number of selections made by the node
	int nSelections_;
	// Cumulative number of sites ever selected
	int nCumulativeSites_;
	// Branch for ForEach (if defined)
	AnalysisSequenceNode* forEachBranch_;
	// Current Site index
	int currentSiteIndex_;

	public:
	// Add "same molecule" exclusion
	bool addSameMoleculeExclusion(AnalysisSelectBaseNode* node);
	// Add "same site" exclusion
	bool addSameSiteExclusion(AnalysisSelectBaseNode* node);
	// Add and return ForEach sequence
	AnalysisSequenceNode* addForEachBranch();
	// Add specified node to ForEach sequence
	void addToForEachBranch(AnalysisNode* node);


	/*
	 * Site Information
	 */
	public:
	// Return the number of available sites in the current stack, if any
	int nSitesInStack() const;
	// Return the average number of sites selected
	double nAverageSites() const;
	// Return the cumulative number of sites ever selected
	int nCumulativeSites() const;
	// Return current site
	const Site* currentSite() const;
};

#endif
