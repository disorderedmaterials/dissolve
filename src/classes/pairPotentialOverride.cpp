// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairPotentialOverride.h"
#include "classes/atomType.h"

PairPotentialOverride::PairPotentialOverride() : interactionPotential_(ShortRangeFunctions::Form::None) {}

PairPotentialOverride::PairPotentialOverride(std::string_view matchI, std::string_view matchJ,
                                             PairPotentialOverride::PairPotentialOverrideType overrideType,
                                             const InteractionPotential<ShortRangeFunctions> &potential)
    : matchI_(matchI), matchJ_(matchJ), type_(overrideType), interactionPotential_(potential)
{
}

// Return enum options for
EnumOptions<PairPotentialOverride::PairPotentialOverrideType> PairPotentialOverride::pairPotentialOverrideTypes()
{
    return EnumOptions<PairPotentialOverride::PairPotentialOverrideType>("PairPotentialOverrideType",
                                                                         {{PairPotentialOverride::Off, "Off"},
                                                                          {PairPotentialOverride::Add, "Add"},
                                                                          {PairPotentialOverride::Replace, "Replace"}});
}

// Set first AtomType name to match
void PairPotentialOverride::setMatchI(std::string_view match) { matchI_ = match; }

// Return first AtomType name to match
std::string_view PairPotentialOverride::matchI() const { return matchI_; }

// Set second AtomType name to match
void PairPotentialOverride::setMatchJ(std::string_view match) { matchJ_ = match; }

// Return second AtomType name to match
std::string_view PairPotentialOverride::matchJ() const { return matchJ_; }

// Set override type
void PairPotentialOverride::setType(PairPotentialOverrideType overrideType) { type_ = overrideType; }

// Return override type
PairPotentialOverride::PairPotentialOverrideType PairPotentialOverride::type() const { return type_; }

// Return interaction potential
InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() { return interactionPotential_; }
const InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() const
{
    return interactionPotential_;
}
