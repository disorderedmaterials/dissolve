/*
    *** Configuration - Sites
    *** src/classes/configuration_sites.cpp
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
