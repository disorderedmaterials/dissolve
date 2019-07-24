/*
	*** Procedure Node - Box
	*** src/procedure/nodes/box.h
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

#ifndef DISSOLVE_PROCEDURENODE_BOX_H
#define DISSOLVE_PROCEDURENODE_BOX_H

#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Box Node
class BoxProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	BoxProcedureNode(Vec3<double> lengths = Vec3<double>(), Vec3<double> angles = Vec3<double>(), bool nonPeriodic = false);
	// Destructor
	~BoxProcedureNode();


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
	enum BoxNodeKeyword { AnglesKeyword, EndBoxKeyword, LengthsKeyword, NonPeriodicKeyword, nBoxNodeKeywords };
	// Return enum option info for BoxNodeKeyword
	static EnumOptions<BoxNodeKeyword> boxNodeKeywords();


	/*
	 * Box Geometry
	 */
	private:
	// Box lengths
	NodeValue lengthA_, lengthB_, lengthC_;
	// Box angles
	NodeValue angleAlpha_, angleBeta_, angleGamma_;
	// Whether Box is non-periodic 
	bool nonPeriodic_;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
