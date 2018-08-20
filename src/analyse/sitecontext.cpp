/*
	*** Site Context
	*** src/analyse/sitecontext.cpp
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

#include "analyse/sitecontext.h"
#include "base/sysfunc.h"

// Constructor
SiteContext::SiteContext()
{
}

// Destructor
SiteContext::~SiteContext()
{
}

/*
 * Stack References
 */

// Add reference to specified SiteStack
SiteReference* SiteContext::add(SiteStack& siteStack, const char* name)
{
	SiteReference* ref = new SiteReference(siteStack, name);

	references_.own(ref);

	return ref;
}

// Retrieve named reference, if it exists
SiteReference* SiteContext::reference(const char* name) const
{
	for (SiteReference* ref = references_.first(); ref != NULL; ref = ref->next) if (DissolveSys::sameString(ref->name(), name)) return ref;

	return NULL;
}
