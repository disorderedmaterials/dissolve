// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "data/elements.h"

// Construct empirical formula from supplied element counts
std::string EmpiricalFormula::constructFormula(const std::vector<int> &elCounts, bool richText)
{
    std::string s;

    // Loop over elements in descending order
    for (auto n = Elements::nElements - 1; n >= 0; --n)
    {
        if (elCounts[n] == 0)
            continue;

        auto Z = Elements::element(n);
        if (elCounts[n] > 1)
        {
            if (richText)
                s += fmt::format("{}<sub>{}</sub>", Elements::symbol(Z), elCounts[n]);
            else
                s += fmt::format("{}{}", Elements::symbol(Z), elCounts[n]);
        }
        else
            s += fmt::format("{}", Elements::symbol(Z));
    }

    return s;
}

// Return empirical formula for supplied Species
std::string EmpiricalFormula::formula(const Species *species, bool richText)
{
    std::vector<int> elCounts(Elements::nElements, 0);

    for (const auto &i : species->atoms())
        ++elCounts[i.Z()];

    return constructFormula(elCounts, richText);
}

// Return empirical formula for supplied SpeciesAtom reflist
std::string EmpiricalFormula::formula(const RefList<SpeciesAtom> &atoms, bool richText)
{
    std::vector<int> elCounts(Elements::nElements, 0);

    for (const auto *i : atoms)
        ++elCounts[i->Z()];

    return constructFormula(elCounts, richText);
}
