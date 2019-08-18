/*
	*** Procedure Node - Dynamic Site
	*** src/procedure/nodes/dynamicsite.h
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

#ifndef DISSOLVE_PROCEDURENODE_DYNAMICSITE_H
#define DISSOLVE_PROCEDURENODE_DYNAMICSITE_H

#include "procedure/nodes/node.h"
#include "templates/array.h"
#include "templates/reflist.h"
#include "templates/refdatalist.h"

// Forward Declarations
class SelectProcedureNode;
class AtomType;
class Element;
class LineParser;
class Molecule;
class NodeScopeStack;

// Procedure Node - Dynamic Site
class DynamicSiteProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	DynamicSiteProcedureNode(SelectProcedureNode* parent);
	// Destructor
	~DynamicSiteProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified context is relevant for this node type
	bool isContextRelevant(ProcedureNode::NodeContext context);


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum DynamicSiteNodeKeyword { AtomTypeKeyword, ElementKeyword, EndDynamicSiteKeyword, nDynamicSiteNodeKeywords };
	// Return enum option info for DynamicSiteNodeKeyword
	static EnumOptions<DynamicSiteNodeKeyword> dynamicSiteNodeKeywords();


	/*
	 * Site Criteria
	 */
	private:
	// Parent Select node for context
	SelectProcedureNode* parent_;
	// Target Elements for selection as sites
	RefList<Element> elements_;
	// Target AtomTypes for selection as sites
	RefDataList<AtomType,int> atomTypes_;


	/*
	 * Site Generation
	 */
	private:
	// List of generated sites
	Array<Site> generatedSites_;

	private:
	// Generate dynamic sites from the specified Molecule
	void generateSites(const Molecule* molecule); 

	public:
	// Return Array of generated sites
	const Array<Site>& generatedSites() const;


	/*
	 * Execute
	 */
	public:
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
