/*
	*** Species Definition - Site
	*** src/classes/species_site.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Add a new SpeciesSite to this Species
SpeciesSite* Species::addSite(const char* name)
{
	SpeciesSite* site = sites_.add();
	site->setParent(this);
	site->setName(uniqueSiteName(name));

	return site;
}

// Remove specified SpeciesSite
void Species::removeSite(SpeciesSite* site)
{
	if (site== NULL) Messenger::error("NULL_POINTER - NULL SpeciesSite passed to Species::removeSite().\n");
	else if (sites_.contains(site))
	{
		CharString tempName = site->name();
		sites_.remove(site);
		Messenger::print("Removed SpeciesSite '%s' from Species '%s'.\n", tempName.get(), name_.get());
	}
	else
	{
		Messenger::print("BAD_REMOVE - Can't remove specified SpeciesSite '%s' from Species '%s' since it doesn't exist.\n", site->name(), name_.get());
		if (!site->parent()) Messenger::print("BAD_CLASS - No parent pointer set in Isotopologue '%s'.\n", site->name());
		else Messenger::print("BAD_REMOVE - Parent Species (%s) of SpeciesSite '%s' is different from this one (%s).\n", site->parent()->name(), site->name(), name());
	}
}

// Return number of defined SpeciesSites
int Species::nSites() const
{
	return sites_.nItems();
}

// Return SpeciesSite List
const List<SpeciesSite>& Species::sites() const
{
	return sites_;
}

// Return nth SpeciesSite defined
SpeciesSite* Species::site(int n)
{
	return sites_[n];
}

// Generate unique site name with base name provided
const char* Species::uniqueSiteName(const char* baseName, SpeciesSite* exclude) const
{
	static CharString uniqueName;
	CharString existingName = baseName;
	int highest = -1;
	
	if (existingName.isEmpty()) existingName = "NewSite";

	// Find all existing names which are the same as 'existingName' up to the first '_', and get the highest appended number
	for (SpeciesSite* site = sites_.first(); site != NULL; site = site->next())
	{
		if (site == exclude) continue;
		if (strcmp(existingName, site->name()) == 0) highest = 0;
		else if (strcmp(existingName,DissolveSys::beforeLastChar(site->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(site->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", existingName.get(), ++highest);
	else uniqueName = existingName;
	
	return uniqueName;
}

// Search for SpeciesSite by name
SpeciesSite* Species::findSite(const char* name) const
{
	for (SpeciesSite* site = sites_.first(); site != NULL; site = site->next()) if (DissolveSys::sameString(name, site->name())) return site;
	return NULL;
}
