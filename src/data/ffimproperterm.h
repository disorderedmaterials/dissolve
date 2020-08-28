/*
    *** Forcefield Improper Term
    *** src/data/ffimproperterm.h
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/parameters.h"
#include "classes/speciesimproper.h"

// Forward Declarations
class Forcefield;
class ForcefieldAtomType;

// Forcefield Improper Term
class ForcefieldImproperTerm
{
    public:
    ForcefieldImproperTerm(std::string_view typeI = "", std::string_view typeJ = "", std::string_view typeK = "",
                           std::string_view typeL = "", SpeciesImproper::ImproperFunction form = SpeciesImproper::NoForm,
                           const std::vector<double> parameters = {});
    ~ForcefieldImproperTerm() = default;

    /*
     * Data
     */
    private:
    // Type names involved in interaction
    std::string typeI_, typeJ_, typeK_, typeL_;
    // Functional form of interaction
    SpeciesImproper::ImproperFunction form_;
    // Parameters for interaction
    std::vector<double> parameters_;

    public:
    // Return if this term matches the atom types supplied
    bool isMatch(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                 const ForcefieldAtomType &l) const;
    // Return functional form index of interaction
    SpeciesImproper::ImproperFunction form() const;
    // Return array of parameters
    const std::vector<double> &parameters() const;
};
