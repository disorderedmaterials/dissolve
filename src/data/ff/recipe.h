// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

// Forward Declarations
class Forcefield;
class Species;

// Forcefield Recipe
struct ForcefieldRecipe
{
    public:
    // Common term recipe
    enum CommonTermRecipe
    {
        None,
        AddAndReuse,
        ClearAndCreateNew
    };

    public:
    // Forcefield to apply
    Forcefield *ff{nullptr};
    // Whether to reduce similar intramolecular definitions to common terms
    CommonTermRecipe commonTerms{CommonTermRecipe::AddAndReuse};

    public:
    // Apply to species
    bool apply(Species &species) const;
};
