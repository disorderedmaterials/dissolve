/*
	*** Analysis Node - Sequence
	*** src/analyse/nodes/sequence.h
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

#ifndef DISSOLVE_ANALYSISSEQUENCE_H
#define DISSOLVE_ANALYSISSEQUENCE_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Sequence Node
class AnalysisSequenceNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisSequenceNode(const char* blockTerminationKeyword = NULL);
	// Destructor
	~AnalysisSequenceNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum SequenceNodeKeyword { nSequenceNodeKeywords };
	// Convert string to node keyword
	static SequenceNodeKeyword sequenceNodeKeyword(const char* s);
	// Convert node keyword to string
	static const char* sequenceNodeKeyword(SequenceNodeKeyword nk);


	/*
	 * Node Contents
	 */
	protected:
	// Sequential node list
	List<AnalysisNode> sequence_;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(Configuration* cfg, const char* dataPrefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	private:
	// Block termination keyword for current context when reading
	CharString blockTerminationKeyword_;

	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
