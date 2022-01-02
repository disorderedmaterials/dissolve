// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <fmt/core.h>

namespace EmpiricalFormula
{
// Return empirical formula for range
template <class Class, class Lam> std::string formula(const Class &range, Lam lambda, bool richText = false)
{
    std::vector<int> elCounts(Elements::nElements, 0);

    for (const auto &obj : range)
    {
        Elements::Element Z = lambda(obj);
        ++elCounts[Z];
    }

    // Loop over elements in descending order and construct formula string
    std::string formula;
    for (auto n = Elements::nElements - 1; n >= 0; --n)
    {
        if (elCounts[n] == 0)
            continue;

        auto Z = Elements::element(n);
        if (elCounts[n] > 1)
        {
            if (richText)
                formula += fmt::format("{}<sub>{}</sub>", Elements::symbol(Z), elCounts[n]);
            else
                formula += fmt::format("{}{}", Elements::symbol(Z), elCounts[n]);
        }
        else
            formula += fmt::format("{}", Elements::symbol(Z));
    }

    return formula;
}
}; // namespace EmpiricalFormula
