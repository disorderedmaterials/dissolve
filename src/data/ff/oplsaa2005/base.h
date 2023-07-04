// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalRef.h"

// OPLS-AA (2005) Base Forcefield
class OPLSAA2005BaseForcefield : public Forcefield
{
    public:
    OPLSAA2005BaseForcefield();
    ~OPLSAA2005BaseForcefield() override;

    /*
     * Definition
     */
    public:
    // Return formatted publication references
    std::string_view publicationReferences() const;
    // Return short-range interaction style for AtomTypes
    ShortRangeFunctions::Form shortRangeForm() const override;

    /*
     * Atom Type Data
     */
    public:
    // Return the base ForcefieldAtomType with specified id (if it exists)
    OptionalReferenceWrapper<const ForcefieldAtomType> oplsAtomTypeById(int id) const;

    /*
     * Term Data
     */
    public:
    // Return bond term for the supplied atom type pair (if it exists)
    OptionalReferenceWrapper<const ForcefieldBondTerm> getBondTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j) const override;
    // Return angle term for the supplied atom type trio (if it exists)
    OptionalReferenceWrapper<const ForcefieldAngleTerm> getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j,
                                                                     const ForcefieldAtomType &k) const override;
    // Return torsion term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType &i,
                                                                         const ForcefieldAtomType &j,
                                                                         const ForcefieldAtomType &k,
                                                                         const ForcefieldAtomType &l) const override;
    // Return improper term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType &i,
                                                                           const ForcefieldAtomType &j,
                                                                           const ForcefieldAtomType &k,
                                                                           const ForcefieldAtomType &l) const override;
};
