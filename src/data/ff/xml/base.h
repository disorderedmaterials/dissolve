// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalref.h"
#include <string_view>

// Forward Declarations
/* none */

// OPLS-AA (2005) Base Forcefield
class Forcefield_XML : public Forcefield
{
    public:
    Forcefield_XML(std::vector<ForcefieldAtomType>, std::vector<ForcefieldBondTerm>, std::vector<ForcefieldAngleTerm>,
                   std::vector<ForcefieldTorsionTerm>, std::vector<ForcefieldImproperTerm>);
    ~Forcefield_XML() = default;

    /*
     * Definition
     */
    public:
    // Prepare the forcefield
    bool setUp();
    // The name of the forcefield
    std::string_view name() const;
    // A description of the forcefield
    std::string_view description() const;
    // Return formatted publication references
    std::string_view publicationReferences() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;

    /*
     * Atom Type Data
     */
    public:
    // Return the base ForcefieldAtomType with specified id (if it exists)
    OptionalReferenceWrapper<const ForcefieldAtomType> oplsAtomTypeById(int id) const;
};
