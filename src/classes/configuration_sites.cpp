// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/species.h"
#include <algorithm>

// Calculate / retrieve stack of sites for specified Species / SpeciesSite
const SiteStack *Configuration::siteStack(const SpeciesSite *site)
{
    // Create or find existing stack in our list
    auto it = std::find_if(siteStacks_.begin(), siteStacks_.end(),
                           [site](const auto &stack) { return stack->speciesSite() == site; });
    if (it == siteStacks_.end())
    {
        siteStacks_.emplace_back(std::make_unique<SiteStack>());
        it = siteStacks_.end() - 1;
    }

    // Recreate the stack list
    if (!it->get()->create(this, site))
    {
        Messenger::error("Failed to create stack for site '{}' in Configuration '{}'.\n", site->name(), name());
        siteStacks_.erase(it);
        return nullptr;
    }

    return it->get();
}
