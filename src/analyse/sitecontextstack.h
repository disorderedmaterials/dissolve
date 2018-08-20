/*
	*** Site Context Stack
	*** src/analyse/sitecontextstack.h
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

#ifndef DISSOLVE_SITECONTEXTSTACK_H
#define DISSOLVE_SITECONTEXTSTACK_H

#include "analyse/sitecontext.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Site Context Stack
class SiteContextStack
{
	public:
	// Constructor
	SiteContextStack();
	// Destructor
	~SiteContextStack();


	/*
	 * Stack
	 */
	private:
	// Site context stack
	List<SiteContext> stack_;

	public:
	// Clear all layers from stack
	void clear();
	// Push new context layer on to the stack
	void push();
	// Pop topmost context layer from the stack
	bool pop();


	/*
	 * Reference Management
	 */
	private:
	// Counter for references added to the stack
	int nReferencesAdded_;

	public:
	// Add new SiteStack reference to the topmost context layer, with name specified
	SiteReference* addToCurrent(SiteStack& siteStack, const char* name);
	// Add dummy reference to the topmost context layer, with name specified 
	SiteReference* addDummyToCurrent(const char* name);
	// Return next available generic name
	const char* nextGenericName() const;
	// Return named reference, if it exists
	SiteReference* reference(const char* name) const;
};

#endif
