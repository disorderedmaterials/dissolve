// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/oplsaa2005/base.h"

// Forward Declarations
/* none */

// OPLS-AA/2005 Alkanes Forcefield
class Forcefield_OPLSAA2005_Protein : public OPLSAA2005BaseForcefield
{
    public:
    Forcefield_OPLSAA2005_Protein() = default;
    ~Forcefield_OPLSAA2005_Protein() = default;

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    bool setUp() {return true;}

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    std::string_view name() const;
    // Return description for Forcefield
    std::string_view description() const;
    // Find the bond terms
    OptionalReferenceWrapper<const ForcefieldBondTerm> getBondTerm(const ForcefieldAtomType&, const ForcefieldAtomType&) const override;
    OptionalReferenceWrapper<const ForcefieldAngleTerm> getAngleTerm(const ForcefieldAtomType&, const ForcefieldAtomType&, const ForcefieldAtomType&) const override;
    OptionalReferenceWrapper<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType&, const ForcefieldAtomType&, const ForcefieldAtomType&, const ForcefieldAtomType&) const override;
    OptionalReferenceWrapper<const ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType&, const ForcefieldAtomType&, const ForcefieldAtomType&, const ForcefieldAtomType&) const override;

};
