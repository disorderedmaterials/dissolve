// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciestorsion.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Torsion Term
class ForcefieldTorsionTerm
{
    public:
    ForcefieldTorsionTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                          std::string_view typeL = "", SpeciesTorsion::TorsionFunction form = SpeciesTorsion::NoForm,
                          const std::vector<double> parameters = {});
    ~ForcefieldTorsionTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_, typeL_;
    // Functional form of interaction
    SpeciesTorsion::TorsionFunction form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                 const ForcefieldAtomType &l) const;
    // Return functional form index of interaction
    SpeciesTorsion::TorsionFunction form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};
