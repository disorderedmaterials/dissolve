/*
	*** Analysis Node - Exclude
	*** src/analyse/nodes/exclude.h
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

#ifndef DISSOLVE_ANALYSISEXCLUDE_H
#define DISSOLVE_ANALYSISEXCLUDE_H

#include "analyse/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisSelectBaseNode;
class LineParser;
class NodeContextStack;

// Analysis Node - Exclude
class AnalysisExcludeNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisExcludeNode();
	// Destructor
	~AnalysisExcludeNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum ExcludeNodeKeyword { EndExcludeKeyword, SameSiteKeyword, nExcludeNodeKeywords };
	// Convert string to control keyword
	static ExcludeNodeKeyword excludeNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* excludeNodeKeyword(ExcludeNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Whether to disallow Sites being the same
	bool disallowSameSite_;
	// Trio of nodes (sites) to compare when disallowing same Sites
	AnalysisSelectBaseNode* sameSiteA_, *sameSiteB_, *sameSiteC_;


	/*
	 * Execute
	 */
	public:
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
