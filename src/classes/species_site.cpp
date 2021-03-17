// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/species.h"
#include <string.h>

// Add a new SpeciesSite to this Species
SpeciesSite *Species::addSite(std::string_view name)
{
    auto &site = sites_.emplace_back();
    site.setParent(this);
    site.setName(uniqueSiteName(name));

    return &site;
}

// Remove specified SpeciesSite
void Species::removeSite(SpeciesSite *site)
{
    auto it = std::remove_if(sites_.begin(), sites_.end(), [&](const auto &p) { return &p == site; });
    if (site == nullptr)
        Messenger::error("NULL_POINTER - NULL SpeciesSite passed to Species::removeSite().\n");
    else if (it != sites_.end())
    {
        Messenger::print("Removed SpeciesSite '{}' from Species '{}'.\n", site->name(), name_);
        sites_.erase(it, sites_.end());
    }
}

// Return number of defined SpeciesSites
int Species::nSites() const { return sites_.size(); }

// Return SpeciesSite List
const std::vector<SpeciesSite> &Species::sites() const { return sites_; }
std::vector<SpeciesSite> &Species::sites() { return sites_; }

// Return nth SpeciesSite defined
SpeciesSite &Species::site(int n) { return sites_[n]; }

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
OptionalReferenceWrapper<const SpeciesSite> Species::findSite(std::string_view name, const SpeciesSite *exclude) const
{
    auto it = std::find_if(sites_.begin(), sites_.end(), [name, exclude](const auto &p) {
        return ((&p != exclude) && (DissolveSys::sameString(name, p.name())));
    });
    if (it != sites_.end())
        return *it;
    else
        return std::nullopt;
}
OptionalReferenceWrapper<SpeciesSite> Species::findSite(std::string_view name, const SpeciesSite *exclude)
{
    auto it = std::find_if(sites_.begin(), sites_.end(), [name, exclude](const auto &p) {
        return ((&p != exclude) && (DissolveSys::sameString(name, p.name())));
    });
    if (it != sites_.end())
        return *it;
    else
        return std::nullopt;
}
