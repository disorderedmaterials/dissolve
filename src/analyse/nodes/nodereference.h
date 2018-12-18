/*
	*** Analysis Node Reference
	*** src/analyse/nodes/nodereference.h
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

#ifndef DISSOLVE_ANALYSISNODEREFERENCE_H
#define DISSOLVE_ANALYSISNODEREFERENCE_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"
#include "templates/listitem.h"

// Forward Declarations
class CoreData;

// Analysis Node Reference
class AnalysisNodeReference : public ListItem<AnalysisNodeReference>
{
	public:
	// Constructor
	AnalysisNodeReference(AnalysisNode* node = NULL);
	// Destructor
	~AnalysisNodeReference();


	/*
	 * Data
	 */
	private:
	// Target node
	AnalysisNode* node_;
	// Allowable node types (when reading / setting)
	bool allowedTypes_[AnalysisNode::nNodeTypes];

	public:
	// Return target node
	AnalysisNode* node();
	// Return node type
	AnalysisNode::NodeType type() const;
	// Add allowable node type
	void addAllowableNodeType(AnalysisNode::NodeType nt);
	// Allow all node types
	void setAllowAllNodeTypes();
	// Return if node pointer is NULL
	bool isNull() const;


	/*
	 * Operators
	 */
	public:
	// Assignment operator
	void operator=(AnalysisNode* node);
	// Assignment operator
	void operator=(const AnalysisNodeReference& nodeRef);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, int startArg, const CoreData& coreData, NodeContextStack& localStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
