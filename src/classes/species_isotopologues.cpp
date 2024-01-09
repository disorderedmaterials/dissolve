// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/species.h"

// Update current Isotopologues
void Species::updateIsotopologues(OptionalReferenceWrapper<const std::vector<std::shared_ptr<AtomType>>> atomTypes)
{
    for (auto &iso : isotopologues_)
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
    auto &iso = isotopologues_.emplace_back(std::make_unique<Isotopologue>());
    iso->setParent(this);
    iso->setName(
        DissolveSys::uniqueName(baseName, isotopologues_, [&](const auto &i) { return iso == i ? std::string() : i->name(); }));
    iso->update();

    return iso.get();
}

// Remove specified Isotopologue from this Species
void Species::removeIsotopologue(Isotopologue *iso)
{
    if (iso == nullptr)
        Messenger::error("NULL_POINTER - NULL Isotopologue passed to Species::removeIsotopologue().\n");
    else if (std::any_of(isotopologues_.begin(), isotopologues_.end(), [iso](auto &i) { return i.get() == iso; }))
    {
        Messenger::print("Removing Isotopologue '{}' from Species '{}'.\n", iso->name(), name_);
        isotopologues_.erase(
            std::remove_if(isotopologues_.begin(), isotopologues_.end(), [iso](auto &i) { return i.get() == iso; }),
            isotopologues_.end());
    }
}

// Return number of defined Isotopologues
int Species::nIsotopologues() const { return isotopologues_.size(); }

// Return nth Isotopologue in the list
Isotopologue *Species::isotopologue(int n) { return isotopologues_[n].get(); }

// Return Isotopologue List
const std::vector<std::unique_ptr<Isotopologue>> &Species::isotopologues() const { return isotopologues_; }

// Return whether the specified Isotopologue exists
bool Species::hasIsotopologue(const Isotopologue *iso) const
{
    return std::any_of(isotopologues_.begin(), isotopologues_.end(), [iso](auto &i) { return i.get() == iso; });
}

// Search for Isotopologue by name
const Isotopologue *Species::findIsotopologue(std::string_view name) const
{
    // Check for the natural Isotopologue
    if (DissolveSys::sameString("Natural", name))
        return naturalIsotopologue();

    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [name](auto &iso) { return DissolveSys::sameString(name, iso->name()); });
    if (it != isotopologues_.end())
        return it->get();

    return nullptr;
}

// Return index of specified Isotopologue
int Species::indexOfIsotopologue(const Isotopologue *iso) const
{
    auto i = std::find_if(isotopologues_.begin(), isotopologues_.end(), [iso](auto &i) { return i.get() == iso; });
    return i - isotopologues_.begin();
}
