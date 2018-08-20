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
#include "classes/sitestack.h"

// Constructor
SiteContextStack::SiteContextStack()
{
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
	stack_.add();
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

// Add new SiteStack reference to the topmost context layer, with name specified
SiteReference* SiteContextStack::addToCurrent(SiteStack& siteStack, const char* name)
{
	// Check that we have a context to add to
	if (stack_.nItems() == 0)
	{
		Messenger::error("No context to add new site reference '%s' to.\n", name);
		return NULL;
	}

	// Check that the name is valid
	if (reference(name))
	{
		Messenger::error("A site reference with name '%s' already exists.\n", name);
		return NULL;
	}

	// Increase the general counter for new references, and add it
	++nReferencesAdded_;

	return stack_.last()->add(siteStack, name);
}

// Add dummy reference to the topmost context layer, with name specified 
SiteReference* SiteContextStack::addDummyToCurrent(const char* name)
{
	static SiteStack dummy;
	return addToCurrent(dummy, name);
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

// Return named reference, if it exists
SiteReference* SiteContextStack::reference(const char* name) const
{
	for (SiteContext* context = stack_.first(); context != NULL; context = context->next)
	{
		SiteReference* ref = context->reference(name);
		if (ref) return ref;
	}

	return NULL;
}
