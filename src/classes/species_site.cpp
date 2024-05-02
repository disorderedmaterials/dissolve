// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/species.h"
#include <cstring>

// Add a new SpeciesSite to this Species
SpeciesSite *Species::addSite(std::string_view name)
{
    auto &site = sites_.emplace_back(std::make_unique<SpeciesSite>(this));
    site->setName(uniqueSiteName(name));

    return site.get();
}

// Remove specified SpeciesSite
void Species::removeSite(SpeciesSite *site)
{
    assert(site);
    auto it = std::find_if(sites_.begin(), sites_.end(), [&](const auto &p) { return p.get() == site; });
    if (it == sites_.end())
        throw(std::runtime_error(fmt::format("Site '{}' doesn't exist in Species '{}'.\n", site->name(), name_)));

    Messenger::print("Removing site '{}' from species '{}'...\n", site->name(), name_);
    sites_.erase(it);
}

// Return number of defined SpeciesSites
int Species::nSites() const { return sites_.size(); }

// Return SpeciesSite List
const std::vector<std::unique_ptr<SpeciesSite>> &Species::sites() const { return sites_; }
std::vector<std::unique_ptr<SpeciesSite>> &Species::sites() { return sites_; }

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
const SpeciesSite *Species::findSite(std::string_view name, const SpeciesSite *exclude) const
{
    auto it = std::find_if(sites_.begin(), sites_.end(),
                           [name, exclude](const auto &site)
                           { return ((site.get() != exclude) && (DissolveSys::sameString(name, site->name()))); });
    if (it != sites_.end())
        return (*it).get();

    return nullptr;
}
SpeciesSite *Species::findSite(std::string_view name, const SpeciesSite *exclude)
{
    auto it = std::find_if(sites_.begin(), sites_.end(),
                           [name, exclude](const auto &site)
                           { return ((site.get() != exclude) && (DissolveSys::sameString(name, site->name()))); });
    if (it != sites_.end())
        return (*it).get();

    return nullptr;
}
