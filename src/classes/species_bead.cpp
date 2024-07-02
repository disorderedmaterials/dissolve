// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/species.h"
#include <cstring>

// Add a new SpeciesBead to this Species
SpeciesBead *Species::addBead(std::string_view name)
{
    auto &bead = beads_.emplace_back(std::make_unique<SpeciesBead>(this));
    bead->setName(uniqueBeadName(name));

    return bead.get();
}

// Remove specified SpeciesBead
void Species::removeBead(SpeciesBead *bead)
{
    assert(bead);
    auto it = std::find_if(beads_.begin(), beads_.end(), [&](const auto &p) { return p.get() == bead; });
    if (it == beads_.end())
        throw(std::runtime_error(fmt::format("Bead '{}' doesn't exist in Species '{}'.\n", bead->name(), name_)));

    Messenger::print("Removing bead '{}' from species '{}'...\n", bead->name(), name_);
    beads_.erase(it);
}

// Return number of defined SpeciesBeads
int Species::nBeads() const { return beads_.size(); }

// Return SpeciesBead List
const std::vector<std::unique_ptr<SpeciesBead>> &Species::beads() const { return beads_; }
std::vector<std::unique_ptr<SpeciesBead>> &Species::beads() { return beads_; }

// Generate unique bead name with base name provided
std::string Species::uniqueBeadName(std::string_view base, const SpeciesBead *exclude) const
{
    std::string_view baseName = base.empty() ? "NewBead" : base;
    std::string uniqueName{baseName};

    // Find all existing names which are the same as 'existingName' up to the first '_', and get the highest appended number
    auto highest = 0;
    while (findBead(uniqueName, exclude))
        uniqueName = fmt::format("{}_{}", baseName, ++highest);

    return uniqueName;
}

// Search for SpeciesBead by name
const SpeciesBead *Species::findBead(std::string_view name, const SpeciesBead *exclude) const
{
    auto it = std::find_if(beads_.begin(), beads_.end(),
                           [name, exclude](const auto &bead)
                           { return ((bead.get() != exclude) && (DissolveSys::sameString(name, bead->name()))); });
    if (it != beads_.end())
        return (*it).get();

    return nullptr;
}
SpeciesBead *Species::findBead(std::string_view name, const SpeciesBead *exclude)
{
    auto it = std::find_if(beads_.begin(), beads_.end(),
                           [name, exclude](const auto &bead)
                           { return ((bead.get() != exclude) && (DissolveSys::sameString(name, bead->name()))); });
    if (it != beads_.end())
        return (*it).get();

    return nullptr;
}
