/*
    *** Empirical Formula Generation
    *** src/classes/empiricalformula.cpp
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

#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "data/elements.h"

// Construct empirical formula from supplied element counts
std::string EmpiricalFormula::constructFormula(const std::vector<int> &elCounts, bool richText)
{
    std::string s;

    // Loop over elements in descending order
    for (auto n = Elements::nElements() - 1; n >= 0; --n)
    {
        if (elCounts[n] == 0)
            continue;
        else if (elCounts[n] > 1)
        {
            if (richText)
                s += fmt::format("{}<sub>{}</sub>", Elements::symbol(n), elCounts[n]);
            else
                s += fmt::format("{}{}", Elements::symbol(n), elCounts[n]);
        }
        else
            s += fmt::format("{}", Elements::symbol(n));
    }

    return s;
}

// Return empirical formula for supplied Species
std::string EmpiricalFormula::formula(const Species *species, bool richText)
{
    std::vector<int> elCounts(Elements::nElements(), 0);

    ListIterator<SpeciesAtom> atomIterator(species->atoms());
    while (SpeciesAtom *i = atomIterator.iterate())
        ++elCounts[i->element()->Z()];

    return constructFormula(elCounts, richText);
}

// Return empirical formula for supplied SpeciesAtom reflist
std::string EmpiricalFormula::formula(const RefList<SpeciesAtom> &atoms, bool richText)
{
    std::vector<int> elCounts(Elements::nElements(), 0);

    for (const auto *i : atoms)
        ++elCounts[i->element()->Z()];

    return constructFormula(elCounts, richText);
}
