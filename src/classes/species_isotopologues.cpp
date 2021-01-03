// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/species.h"
#include <string.h>

// Update current Isotopologues
void Species::updateIsotopologues(OptionalReferenceWrapper<const std::vector<std::shared_ptr<AtomType>>> atomTypes)
{
    for (Isotopologue *iso = isotopologues_.first(); iso != nullptr; iso = iso->next())
    {
        if (atomTypes)
            iso->checkAtomTypes(*atomTypes);

        iso->update();
    }
}

// Update and return natural isotopologue
const Isotopologue *Species::naturalIsotopologue() const { return &naturalIsotopologue_; }

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
const Isotopologue *Species::findIsotopologue(std::string_view name, const Isotopologue *exclude) const
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
