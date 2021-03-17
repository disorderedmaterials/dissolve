// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/species.h"

// Calculate / retrieve stack of sites for specified Species / SpeciesSite
const SiteStack *Configuration::siteStack(SpeciesSite *site)
{
    // Create or find existing stack in our list
    SiteStack *stack = nullptr;
    ListIterator<SiteStack> stackIterator(siteStacks_);
    while ((stack = stackIterator.iterate()))
        if (stack->speciesSite() == site)
            break;
    if (!stack)
        stack = siteStacks_.add();

    // Recreate the stack list
    if (!stack->create(this, site))
    {
        Messenger::error("Failed to create stack for site '{}' in Configuration '{}'.\n", site->name(), name());
        siteStacks_.remove(stack);
        return nullptr;
    }

    return stack;
}
