// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
#include <memory>
#include <string>

// PairPotential Override Definition
class PairPotentialOverride
{
    public:
    // Override Types
    enum PairPotentialOverrideType
    {
        Off,    /* No override will be applied */
        Add,    /* Contributions from the override will be added to the existing potential */
        Replace /* The override will replace any existing potential */
    };
    // Return enum options for PairPotentialOverrideTypes
    static EnumOptions<PairPotentialOverrideType> pairPotentialOverrideTypes();
    PairPotentialOverride();
    PairPotentialOverride(std::string_view matchI, std::string_view matchJ,
                          PairPotentialOverride::PairPotentialOverrideType overrideType,
                          const InteractionPotential<ShortRangeFunctions> &potential);

    private:
    // AtomType names to match
    std::string matchI_{"*"}, matchJ_{"*"};
    // Override type
    PairPotentialOverrideType type_{PairPotentialOverrideType::Add};
    // Interaction potential
    InteractionPotential<ShortRangeFunctions> interactionPotential_;

    public:
    // Set first AtomType name to match
    void setMatchI(std::string_view match);
    // Return first AtomType name to match
    std::string_view matchI() const;
    // Set second AtomType name to match
    void setMatchJ(std::string_view match);
    // Return second AtomType name to match
    std::string_view matchJ() const;
    // Set override type
    void setType(PairPotentialOverrideType overrideType);
    // Return override type
    PairPotentialOverrideType type() const;
    // Return interaction potential
    InteractionPotential<ShortRangeFunctions> &interactionPotential();
    const InteractionPotential<ShortRangeFunctions> &interactionPotential() const;
};
