// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/recipe.h"
#include "data/ff/ff.h"
#include "classes/species.h"

// Apply to species
bool ForcefieldRecipe::apply(Species &species) const
{
    if (!ff)
        return Messenger::error("No forcefield supplied!\n");

    // Clear any existing forcefield data from the species
    species.clearIntraMolecularForcefieldTerms();
    species.clearAtomTypes();
    if (commonTerms != AddAndReuse)
        species.clearCommonTerms();

    // Assign atom types to the species
    if (!ff->assignAtomTypes(this, Forcefield::TypeAll, false).empty())
        return false;

    // Assign intramolecular terms
    if (!ff->assignIntramolecular(this))
        return false;

    return true;
}