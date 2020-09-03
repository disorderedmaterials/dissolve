/*
    *** Empirical Formula Generation
    *** src/classes/empiricalformula.h
    Copyright T. Youngs 2012-2020

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

#include "data/elements.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class SpeciesAtom;

// Empirical Formula Generator
class EmpiricalFormula
{
    private:
    // Construct empirical formula from supplied element counts
    static std::string constructFormula(const std::vector<int> &elCounts, bool richText = false);

    public:
    // Return empirical formula for supplied Species
    static std::string formula(const Species *species, bool richText = false);
    // Return empirical formula for supplied SpeciesAtom reflist
    static std::string formula(const RefList<SpeciesAtom> &atoms, bool richText = false);
};
