/*
	*** Analysis Node - DynamicSelect
	*** src/analyse/nodes/dynamicselect.h
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

#ifndef DISSOLVE_ANALYSISDYNAMICSELECT_H
#define DISSOLVE_ANALYSISDYNAMICSELECT_H

#include "analyse/nodes/node.h"
#include "analyse/nodes/selectbase.h"
#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisSequenceNode;
class Element;
class SiteStack;
class Species;
class SpeciesSite;

// Dynamic Select Node
class AnalysisDynamicSelectNode : public AnalysisSelectBaseNode, public AnalysisNode
{
	public:
	// Constructor
	AnalysisDynamicSelectNode();
	// Destructor
	~AnalysisDynamicSelectNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum DynamicSelectNodeKeyword { ElementsKeyword, EndDynamicSelectKeyword, SiteKeyword, nDynamicSelectNodeKeywords };
	// Convert string to control keyword
	static DynamicSelectNodeKeyword dynamicSelectNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* dynamicSelectNodeKeyword(DynamicSelectNodeKeyword nk);


	/*
	 * Selection Target
	 */
	private:
	// Array of dynamically-created sites
	Array<Site> dynamicSites_;
	// Target elements for selection
	RefList<Element,bool> targetElements_;
	
	public:
	// Add elemental selection target
	bool addElementTarget(Element* el);


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


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
