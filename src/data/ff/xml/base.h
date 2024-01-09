// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalRef.h"
#include <string_view>

// Forcefield whose terms are supplied by a user supplied OpenMM xml file
class Forcefield_XML : public Forcefield
{
    public:
    Forcefield_XML(std::string name, std::vector<ForcefieldAtomType> &&atoms, std::vector<ForcefieldBondTerm> &&bonds,
                   std::vector<ForcefieldAngleTerm> &&angles, std::vector<ForcefieldTorsionTerm> &&torsions,
                   std::vector<ForcefieldImproperTerm> &&impropers);
    ~Forcefield_XML() override = default;

    private:
    std::string name_;

    /*
     * Definition
     */
    public:
    // Prepare the forcefield
    bool setUp() override;
    // The name of the forcefield
    std::string_view name() const override;
    // A description of the forcefield
    std::string_view description() const override;
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
};
