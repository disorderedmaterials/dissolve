// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairPotentialOverride.h"
#include "classes/atomType.h"

PairPotentialOverride::PairPotentialOverride() : interactionPotential_(ShortRangeFunctions::Form::None) {}

// Return enum options for
EnumOptions<PairPotentialOverride::PairPotentialOverrideTypes> PairPotentialOverride::pairPotentialOverrideTypes()
{
    return EnumOptions<PairPotentialOverride::PairPotentialOverrideTypes>("PairPotentialOverrideTypes",
                                                                          {{PairPotentialOverride::Off, "Off"},
                                                                           {PairPotentialOverride::Add, "Add"},
                                                                           {PairPotentialOverride::Replace, "Replace"}});
}

// Set atom type pair for which the override is relevant
bool PairPotentialOverride::setAtomTypes(const std::shared_ptr<AtomType> &typeI, const std::shared_ptr<AtomType> &typeJ)
{
    atomTypeI_ = typeI;
    atomTypeJ_ = typeJ;
}

// Return first AtomType name
std::string_view PairPotentialOverride::atomTypeNameI() const
{
    assert(atomTypeI_);
    return atomTypeI_->name();
}

// Return second AtomType name
std::string_view PairPotentialOverride::atomTypeNameJ() const
{
    assert(atomTypeJ_);
    return atomTypeJ_->name();
}

// Return first source AtomType
std::shared_ptr<AtomType> PairPotentialOverride::atomTypeI() const { return atomTypeI_; }

// Return second source AtomType
std::shared_ptr<AtomType> PairPotentialOverride::atomTypeJ() const { return atomTypeJ_; }

// Return interaction potential
InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() { return interactionPotential_; }
const InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() const
{
    return interactionPotential_;
}
