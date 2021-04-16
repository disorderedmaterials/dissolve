// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalref.h"

// Forward Declarations
/* none */

// OPLS-AA (2005) Base Forcefield
class OPLSAA2005BaseForcefield : public Forcefield
{
    public:
    OPLSAA2005BaseForcefield();
    virtual ~OPLSAA2005BaseForcefield();

    /*
     * Definition
     */
    public:
    // Return formatted publication references
    std::string_view publicationReferences() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const override;

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
                                                                   const ForcefieldAtomType &j) const;
    // Return angle term for the supplied atom type trio (if it exists)
    OptionalReferenceWrapper<const ForcefieldAngleTerm> getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j,
                                                                     const ForcefieldAtomType &k) const;
    // Return torsion term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType &i,
                                                                         const ForcefieldAtomType &j,
                                                                         const ForcefieldAtomType &k,
                                                                         const ForcefieldAtomType &l) const;
    // Return improper term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType &i,
                                                                           const ForcefieldAtomType &j,
                                                                           const ForcefieldAtomType &k,
                                                                           const ForcefieldAtomType &l) const;
};
