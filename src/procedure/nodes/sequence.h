/*
	*** Procedure Node - Sequence
	*** src/procedure/nodes/sequence.h
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

#ifndef DISSOLVE_PROCEDURENODE_SEQUENCE_H
#define DISSOLVE_PROCEDURENODE_SEQUENCE_H

#include "procedure/nodes/node.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Sequence Node
class SequenceProcedureNode : public ProcedureNode
{
	public:
	// Constructor
	SequenceProcedureNode(ProcedureNode::NodeContext context, const char* blockTerminationKeyword = NULL);
	// Destructor
	~SequenceProcedureNode();


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
	enum SequenceNodeKeyword { nSequenceNodeKeywords };
	// Return enum option info for SequenceNodeKeyword
	static EnumOptions<SequenceNodeKeyword> sequenceNodeKeywords();


	/*
	 * Node Contents
	 */
	protected:
	// Sequential node list
	List<ProcedureNode> sequence_;
	// Context of the sequence
	ProcedureNode::NodeContext context_;

	public:
	// Clear all data
	void clear();
	// Add (own) node into sequence
	void addNode(ProcedureNode* node);


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* prefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	ProcedureNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	private:
	// Block termination keyword for current context when reading
	CharString blockTerminationKeyword_;

	public:
	// Set block termination keyword for current context when reading
	void setBlockTerminationKeyword(const char* endKeyword);
	// Return block termination keyword for current context
	const char* blockTerminationKeyword() const;
	// Read structure from specified LineParser
	bool read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
