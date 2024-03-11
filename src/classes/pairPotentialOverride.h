// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include <memory>

// Forward Declarations
class AtomType;

// PairPotential Override Definition
class PairPotentialOverride
{
    public:
    PairPotentialOverride();
    // Override Types
    enum PairPotentialOverrideTypes
    {
        Off,    /* No override will be applied */
        Add,    /* Contributions from the override will be added to the existing potential */
        Replace /* The override will replace any existing potential */
    };
    // Return enum options for PairPotentialOverrideTypes
    static EnumOptions<PairPotentialOverrideTypes> pairPotentialOverrideTypes();

    private:
    // Original source AtomTypes
    std::shared_ptr<AtomType> atomTypeI_, atomTypeJ_;
    // Interaction potential
    InteractionPotential<ShortRangeFunctions> interactionPotential_;

    public:
    // Set atom type pair for which the override is relevant
    bool setAtomTypes(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ);
    // Return first AtomType name
    std::string_view atomTypeNameI() const;
    // Return second AtomType name
    std::string_view atomTypeNameJ() const;
    // Return first source AtomType
    std::shared_ptr<AtomType> atomTypeI() const;
    // Return second source AtomType
    std::shared_ptr<AtomType> atomTypeJ() const;
    // Return interaction potential
    InteractionPotential<ShortRangeFunctions> &interactionPotential();
    const InteractionPotential<ShortRangeFunctions> &interactionPotential() const;
};
