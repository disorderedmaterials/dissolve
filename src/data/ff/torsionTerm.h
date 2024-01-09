// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesTorsion.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Torsion Term
class ForcefieldTorsionTerm
{
    public:
    ForcefieldTorsionTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                          std::string_view typeL = "", TorsionFunctions::Form form = TorsionFunctions::Form::None,
                          std::string_view parameterString = "", double q14Scale = 0.5, double vdw14Scale = 0.5);
    ~ForcefieldTorsionTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_, typeL_;
    // Functional form of interaction
    TorsionFunctions::Form form_;
    // Parameters for interaction
    std::vector<double> parameters_;
    // Electrostatic and van der Waals 1-4 scale factors
    double q14Scale_, vdw14Scale_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                 const ForcefieldAtomType &l) const;
    // Return functional form index of interaction
    TorsionFunctions::Form form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};
