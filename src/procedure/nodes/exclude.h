/*
	*** Procedure Node - Exclude
	*** src/procedure/nodes/exclude.h
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

#ifndef DISSOLVE_PROCEDURENODE_EXCLUDE_H
#define DISSOLVE_PROCEDURENODE_EXCLUDE_H

#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class SelectProcedureNode;
class LineParser;
class NodeContextStack;

// Procedure Node - Exclude
class ExcludeProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	ExcludeProcedureNode();
	// Destructor
	~ExcludeProcedureNode();


	/*
	 * Identity
	 */
	public:
	// Return whether specified usage type is allowed for this node
	bool isUsageTypeAllowed(ProcedureNode::NodeUsageType usageType);


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum ExcludeNodeKeyword { EndExcludeKeyword, SameSiteKeyword, nExcludeNodeKeywords };
	// Return enum option info for ExcludeNodeKeyword
	static EnumOptions<ExcludeNodeKeyword> excludeNodeKeywords();


	/*
	 * Data
	 */
	private:
	// Whether to disallow Sites being the same
	bool disallowSameSite_;
	// Trio of nodes (sites) to compare when disallowing same Sites
	SelectProcedureNode* sameSiteA_, *sameSiteB_, *sameSiteC_;


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
	bool read(LineParser& parser, const CoreData& coreData, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
