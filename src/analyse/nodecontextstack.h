/*
	*** Node Context Stack
	*** src/analyse/nodecontextstack.h
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

#ifndef DISSOLVE_NODECONTEXTSTACK_H
#define DISSOLVE_NODECONTEXTSTACK_H

#include "base/charstring.h"
#include "templates/array.h"
#include "templates/reflist.h"

// Forward Declarations
class AnalysisSelectNode;
class AnalysisCalculateNode;

// Node Context Stack
class NodeContextStack
{
	public:
	// Constructor
	NodeContextStack();
	// Destructor
	~NodeContextStack();


	/*
	 * Context Stacks
	 */
	private:
	// Counter for SelectNode added to the stack at any point
	int nSelectNodesAdded_;
	// SelectNode context stack
	Array< RefList<AnalysisSelectNode,bool> > selectStack_;
	// CalculateNode context stack
	Array< RefList<AnalysisCalculateNode,bool> > calculateStack_;

	public:
	// Clear all layers from stack
	void clear();
	// Push new context layer on to the stack
	void push();
	// Pop topmost context layer from the stack
	bool pop();
	// Add new selection node the topmost context layer
	bool add(AnalysisSelectNode* selectNode);
	// Add new calculation node the topmost context layer
	bool add(AnalysisCalculateNode* calculateNode);


	/*
	 * Node Search
	 */
	public:
	// Return next available generic name for a SelectNode
	const char* nextSelectName() const;
	// Return if named Select node exists somewhere on the stack
	bool hasSelectNode(const char* name) const;
	// Return named Select node (if it exists)
	AnalysisSelectNode* selectNode(const char* name) const;
	// Return if named Calculate node exists somewhere on the stack
	bool hasCalculateNode(const char* name) const;
	// Return named Calculate node (if it exists)
	AnalysisCalculateNode* calculateNode(const char* name) const;
};

#endif
