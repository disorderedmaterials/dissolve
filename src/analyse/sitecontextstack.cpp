/*
	*** Site Context Stack
	*** src/analyse/sitecontextstack.cpp
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

#include "analyse/sitecontextstack.h"
#include "base/sysfunc.h"

// Constructor
SiteContextStack::SiteContextStack()
{
	nReferencesAdded_ = 0;
}

// Destructor
SiteContextStack::~SiteContextStack()
{
}

/*
 * Stack
 */

// Clear all layers from stack
void SiteContextStack::clear()
{
	stack_.clear();

	nReferencesAdded_ = 0;
}

// Push new context layer on to the stack
void SiteContextStack::push()
{
	stack_.add(RefList<AnalysisNode,CharString>());
}

// Pop topmost context layer from the stack
bool SiteContextStack::pop()
{
	if (stack_.nItems() == 0) return Messenger::error("Site context stack is empty, so nothing to pop().\n");

	stack_.removeLast();

	return true;
}

/*
 * Reference Management
 */

// Add new node/site reference to the topmost context layer, with name specified
bool SiteContextStack::addToCurrent(AnalysisNode* localNode, const char* name)
{
	// Check that we have a context to add to
	if (stack_.nItems() == 0)
	{
		Messenger::error("No context to add new site reference '%s' to.\n", name);
		return false;
	}

	// Check that the name is valid
	if (hasSite(name))
	{
		Messenger::error("A site reference with name '%s' already exists.\n", name);
		return false;
	}

	// Increase the general counter for new references, and add it
	++nReferencesAdded_;

	stack_.last().add(localNode, name);

	return true;
}

// Return next available generic name
const char* SiteContextStack::nextGenericName() const
{
	// Generic names range from A-Z, then AA through to AZ etc. (should we ever need that many)
	static CharString result;
	int nAZ = nReferencesAdded_ / 26;
	if (nAZ == 0) result.sprintf("%c", 65+nReferencesAdded_);
	else result.sprintf("%c%c", 64+nAZ, 65+(nReferencesAdded_%26));

	return result.get();
}

// Return if named site exists somewhere on the stack
bool SiteContextStack::hasSite(const char* name) const
{
	for (int n=0; n<stack_.nItems(); ++n)
	{
		RefListIterator<AnalysisNode,CharString> contextIterator(stack_.constValue(n));
		while (AnalysisNode* node = contextIterator.iterate()) if (DissolveSys::sameString(contextIterator.currentData(), name)) return true;
	}

	return false;
}

	// Return node for named site (if it exists)
AnalysisNode* SiteContextStack::siteNode(const char* name) const
{
	for (int n=0; n<stack_.nItems(); ++n)
	{
		RefListIterator<AnalysisNode,CharString> contextIterator(stack_.constValue(n));
		while (AnalysisNode* node = contextIterator.iterate()) if (DissolveSys::sameString(contextIterator.currentData(), name)) return node;
	}

	return NULL;
}
