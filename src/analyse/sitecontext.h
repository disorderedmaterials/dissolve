/*
	*** Site Context
	*** src/analyse/sitecontext.h
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

#ifndef DISSOLVE_SITECONTEXT_H
#define DISSOLVE_SITECONTEXT_H

#include "analyse/sitereference.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
class SiteStack;

// Site Context
class SiteContext : public ListItem<SiteContext>
{
	public:
	// Constructor
	SiteContext();
	// Destructor
	~SiteContext();


	/*
	 * Stack References
	 */
	private:
	// Site context stack
	List<SiteReference> references_;

	public:
	// Add reference to specified SiteStack
	SiteReference* add(SiteStack& siteStack, const char* name);
	// Retrieve named reference, if it exists
	SiteReference* reference(const char* name) const;
};

#endif
