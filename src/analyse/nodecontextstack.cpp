/*
	*** Node Context Stack
	*** src/analyse/nodecontextstack.cpp
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

#include "analyse/nodecontextstack.h"
#include "analyse/nodes/calculate.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/collect2d.h"
#include "analyse/nodes/collect3d.h"
#include "analyse/nodes/select.h"
#include "base/sysfunc.h"

// Constructor
NodeContextStack::NodeContextStack()
{
	nSelectNodesAdded_ = 0;
}

// Destructor
NodeContextStack::~NodeContextStack()
{
}

/*
 * Stack
 */

// Clear all layers from stack
void NodeContextStack::clear()
{
	selectStack_.clear();
	calculateStack_.clear();
	collect1DNodes_.clear();

	nSelectNodesAdded_ = 0;
}

// Push new context layer on to the stack
void NodeContextStack::push()
{
	selectStack_.add(RefList<AnalysisSelectNode,CharString>());
	calculateStack_.add(RefList<AnalysisCalculateNode,bool>());
}

// Pop topmost context layer from the stack
bool NodeContextStack::pop()
{
	if (selectStack_.nItems() == 0) return Messenger::error("Select context stack is empty, so nothing to pop().\n");
	if (calculateStack_.nItems() == 0) return Messenger::error("Calculate context stack is empty, so nothing to pop().\n");

	selectStack_.removeLast();
	calculateStack_.removeLast();

	return true;
}

// Add new selection node to the topmost context layer
bool NodeContextStack::add(AnalysisSelectNode* selectNode, const char* name)
{
	// Check that we have a context to add to
	if (selectStack_.nItems() == 0)
	{
		Messenger::error("No current context in which to add Select node '%s'.\n", name);
		return false;
	}

	// Check that the name is valid
	if (selectNodeInScope(name))
	{
		Messenger::error("A Select node with name '%s' is already in scope.\n", name);
		return false;
	}

	// Increase the general counter for new references, and add it
	++nSelectNodesAdded_;

	selectStack_.last().add(selectNode, name);
	selectNodes_.add(selectNode, name);

	return true;
}

// Add new calculation node to the topmost context layer
bool NodeContextStack::add(AnalysisCalculateNode* calculateNode)
{
	// Check that we have a context to add to
	if (calculateStack_.nItems() == 0)
	{
		Messenger::error("No current context in which to add Calculate node '%s'.\n", calculateNode->name());
		return false;
	}

	// Check that the name is valid
	if (calculateNodeInScope(calculateNode->name()))
	{
		Messenger::error("A Calculate node with name '%s' already exists.\n", calculateNode->name());
		return false;
	}

	calculateStack_.last().add(calculateNode);

	return true;
}

// Add reference to one-dimensional collect node
void NodeContextStack::add(AnalysisCollect1DNode* collect1DNode)
{
	collect1DNodes_.addUnique(collect1DNode);
}

// Add reference to two-dimensional collect node
void NodeContextStack::add(AnalysisCollect2DNode* collect2DNode)
{
	collect2DNodes_.addUnique(collect2DNode);
}

// Add reference to three-dimensional collect node
void NodeContextStack::add(AnalysisCollect3DNode* collect3DNode)
{
	collect3DNodes_.addUnique(collect3DNode);
}

/*
 * Node Search
 */

// Return next available generic name for a SelectNode
const char* NodeContextStack::nextSelectName() const
{
	// Generic names range from A-Z, then AA through to AZ etc. (should we ever need that many)
	static CharString result;
	int nAZ = nSelectNodesAdded_ / 26;
	if (nAZ == 0) result.sprintf("%c", 65+nSelectNodesAdded_);
	else result.sprintf("%c%c", 64+nAZ, 65+(nSelectNodesAdded_%26));

	return result.get();
}

// Return named Select node if it is currently in scope
AnalysisSelectNode* NodeContextStack::selectNodeInScope(const char* name) const
{
	for (int n=0; n<selectStack_.nItems(); ++n)
	{
		RefListIterator<AnalysisSelectNode,CharString> contextIterator(selectStack_.constAt(n));
		while (AnalysisSelectNode* node = contextIterator.iterate()) if (DissolveSys::sameString(contextIterator.currentData(), name)) return node;
	}

	return NULL;
}

// Return named Calculate node if it is currently in scope
AnalysisCalculateNode* NodeContextStack::calculateNodeInScope(const char* name) const
{
	for (int n=0; n<calculateStack_.nItems(); ++n)
	{
		RefListIterator<AnalysisCalculateNode,bool> contextIterator(calculateStack_.constAt(n));
		while (AnalysisCalculateNode* node = contextIterator.iterate()) if (DissolveSys::sameString(node->name(), name)) return node;
	}

	return NULL;
}

// Return named Collect1D node (if it exists)
AnalysisCollect1DNode* NodeContextStack::collect1DNode(const char* name) const
{
	RefListIterator<AnalysisCollect1DNode,bool> nodeIterator(collect1DNodes_);
	while (AnalysisCollect1DNode* node = nodeIterator.iterate()) if (DissolveSys::sameString(node->name(), name)) return node;

	return NULL;
}

// Return named Collect2D node (if it exists)
AnalysisCollect2DNode* NodeContextStack::collect2DNode(const char* name) const
{
	RefListIterator<AnalysisCollect2DNode,bool> nodeIterator(collect2DNodes_);
	while (AnalysisCollect2DNode* node = nodeIterator.iterate()) if (DissolveSys::sameString(node->name(), name)) return node;

	return NULL;
}

// Return named Collect1D node (if it exists)
AnalysisCollect3DNode* NodeContextStack::collect3DNode(const char* name) const
{
	RefListIterator<AnalysisCollect3DNode,bool> nodeIterator(collect3DNodes_);
	while (AnalysisCollect3DNode* node = nodeIterator.iterate()) if (DissolveSys::sameString(node->name(), name)) return node;

	return NULL;
}


// Return named Select node (if it exists)
AnalysisSelectNode* NodeContextStack::selectNode(const char* name) const
{
	RefListIterator<AnalysisSelectNode,CharString> nodeIterator(selectNodes_);
	while (AnalysisSelectNode* node = nodeIterator.iterate()) if (DissolveSys::sameString(nodeIterator.currentData(), name)) return node;

	return NULL;
}
