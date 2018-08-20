/*
	*** Analysis Base Node
	*** src/analyse/nodes/node.h
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

#ifndef DISSOLVE_ANALYSISNODE_H
#define DISSOLVE_ANALYSISNODE_H

#include "templates/listitem.h"

// Forward Declarations
class LineParser;

// Analysis Base Node
class AnalysisNode : public ListItem<AnalysisNode>
{
	public:
	// Constructor
	AnalysisNode();
	// Destructor
	virtual ~AnalysisNode();


	/*
	 * Node Types
	 */
	public:
	// Available Node Types
	enum NodeType { SelectNode, SequenceNode, nNodeTypes };
	// Convert string to node type
	static NodeType nodeType(const char* s);
	// Convert node type to string
	static const char* nodeType(NodeType nt);

	protected:
	// Node type
	NodeType type_;

	public:
	// Return node type
	NodeType type() const;


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	virtual bool read(LineParser& parser) = 0;
	// Write structure to specified LineParser
	virtual bool write(LineParser& parser, const char* prefix) = 0;
};

#endif
