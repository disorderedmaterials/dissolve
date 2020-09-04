/*
    *** Species Definition - Site
    *** src/classes/species_site.cpp
    Copyright T. Youngs 2012-2020

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

#include "base/sysfunc.h"
#include "classes/species.h"
#include <string.h>

// Add a new SpeciesSite to this Species
SpeciesSite *Species::addSite(std::string_view name)
{
    auto *site = sites_.add();
    site->setParent(this);
    site->setName(uniqueSiteName(name));

    return site;
}

// Remove specified SpeciesSite
void Species::removeSite(SpeciesSite *site)
{
    if (site == nullptr)
        Messenger::error("NULL_POINTER - NULL SpeciesSite passed to Species::removeSite().\n");
    else if (sites_.contains(site))
    {
        Messenger::print("Removed SpeciesSite '{}' from Species '{}'.\n", site->name(), name_);
        sites_.remove(site);
    }
}

// Return number of defined SpeciesSites
int Species::nSites() const { return sites_.nItems(); }

// Return SpeciesSite List
const List<SpeciesSite> &Species::sites() const { return sites_; }

// Return nth SpeciesSite defined
SpeciesSite *Species::site(int n) { return sites_[n]; }

// Generate unique site name with base name provided
std::string Species::uniqueSiteName(std::string_view base, const SpeciesSite *exclude) const
{
    std::string_view baseName = base.empty() ? "NewSite" : base;
    std::string uniqueName{baseName};

    // Find all existing names which are the same as 'existingName' up to the first '_', and get the highest appended number
    auto highest = 0;
    while (findSite(uniqueName, exclude))
        uniqueName = fmt::format("{}_{}", baseName, ++highest);

    return uniqueName;
}

// Search for SpeciesSite by name
SpeciesSite *Species::findSite(std::string_view name, const SpeciesSite *exclude) const
{
    for (auto *site = sites_.first(); site != nullptr; site = site->next())
        if (site == exclude)
            continue;
        else if (DissolveSys::sameString(name, site->name()))
            return site;

    return nullptr;
}
