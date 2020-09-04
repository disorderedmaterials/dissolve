/*
    *** Species Definition - Isotopologues
    *** src/classes/species_isotopologues.cpp
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

// Update current Isotopologues
void Species::updateIsotopologues()
{
    for (auto *iso = isotopologues_.first(); iso != nullptr; iso = iso->next())
        iso->update();
}

// Update and return natural isotopologue
Isotopologue *Species::naturalIsotopologue()
{
    if (naturalIsotopologuePoint_ != atomTypesVersion_)
    {
        naturalIsotopologue_.update();

        naturalIsotopologuePoint_ = atomTypesVersion_;
    }

    return &naturalIsotopologue_;
}

// Add a new Isotopologue to this species
Isotopologue *Species::addIsotopologue(std::string_view baseName)
{
    Isotopologue *iso = isotopologues_.add();
    iso->setParent(this);
    iso->setName(uniqueIsotopologueName(baseName));
    iso->update();

    return iso;
}

// Remove specified Isotopologue from this Species
void Species::removeIsotopologue(Isotopologue *iso)
{
    if (iso == nullptr)
        Messenger::error("NULL_POINTER - NULL Isotopologue passed to Species::removeIsotopologue().\n");
    else if (isotopologues_.contains(iso))
    {
        Messenger::print("Removing Isotopologue '{}' from Species '{}'.\n", iso->name(), name_);
        isotopologues_.remove(iso);
    }
}

// Return number of defined Isotopologues
int Species::nIsotopologues() const { return isotopologues_.nItems(); }

// Return nth Isotopologue in the list
Isotopologue *Species::isotopologue(int n) { return isotopologues_[n]; }

// Return Isotopologue List
const List<Isotopologue> &Species::isotopologues() const { return isotopologues_; }

// Return whether the specified Isotopologue exists
bool Species::hasIsotopologue(const Isotopologue *iso) const { return isotopologues_.contains(iso); }

// Generate unique Isotopologue name with base name provided
std::string Species::uniqueIsotopologueName(std::string_view base, const Isotopologue *exclude)
{
    std::string_view baseName = base.empty() ? "Unnamed" : base;
    std::string uniqueName{baseName};

    auto suffix = 0;
    while (findIsotopologue(uniqueName, exclude))
        uniqueName = fmt::format("{}{}", baseName, ++suffix);

    return uniqueName;
}

// Search for Isotopologue by name
Isotopologue *Species::findIsotopologue(std::string_view name, const Isotopologue *exclude)
{
    // Check for the natural Isotopologue
    if (DissolveSys::sameString("Natural", name))
        return naturalIsotopologue();

    for (auto *iso = isotopologues_.first(); iso != nullptr; iso = iso->next())
        if (iso == exclude)
            continue;
        else if (DissolveSys::sameString(name, iso->name()))
            return iso;

    return nullptr;
}

// Return index of specified Isotopologue
int Species::indexOfIsotopologue(const Isotopologue *iso) const { return isotopologues_.indexOf(iso); }
