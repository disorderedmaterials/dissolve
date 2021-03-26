// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalref.h"
#include <string_view>

// Forward Declarations
/* none */

// Forcefield whose terms are supplied by a user supplied OpenMM xml file
class Forcefield_XML : public Forcefield
{
    public:
    Forcefield_XML(std::string name, std::vector<ForcefieldAtomType> &&atoms, std::vector<ForcefieldBondTerm> &&bonds,
                   std::vector<ForcefieldAngleTerm> &&angles, std::vector<ForcefieldTorsionTerm> &&torsions,
                   std::vector<ForcefieldImproperTerm> &&impropers);
    ~Forcefield_XML() = default;

    private:
    std::string name_;

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
