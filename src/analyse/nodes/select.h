/*
	*** Analysis Node - Select
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

#ifndef DISSOLVE_ANALYSISSELECT_H
#define DISSOLVE_ANALYSISSELECT_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"

// Forward Declarations
class AnalysisSequenceNode;
class SiteStack;
class Species;
class SpeciesSite;

// Select Node
class AnalysisSelectNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisSelectNode();
	// Destructor
	~AnalysisSelectNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum SelectNodeKeyword { EndSelectKeyword, ForEachKeyword, SiteKeyword, nSelectNodeKeywords };
	// Convert string to control keyword
	static SelectNodeKeyword selectNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* selectNodeKeyword(SelectNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Species in which the site is located
	Species* species_;
	// Target site within parent Species
	SpeciesSite* speciesSite_;
	// Stack containing our selected sites
	const SiteStack* siteStack_;
	// Branch for ForEach (if defined)
	AnalysisSequenceNode* forEachBranch_;
	// Current Site index in ForEach branch
	int currentForEachSite_;


	/*
	 * Site Information
	 */
	public:
	// Return whether the node has available site information
	bool hasSites() const;
	// Return the number of available sites, if any
	int nSites() const;
	// Return current site
	const Site& currentSite() const;


	/*
	 * Execute
	 */
	public:
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(Configuration* cfg);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, SiteContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
