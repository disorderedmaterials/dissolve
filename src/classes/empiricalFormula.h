// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <fmt/format.h>
#include <vector>

namespace EmpiricalFormula
{
using EmpiricalFormulaMap = std::map<Elements::Element, int>;

// Return empirical formula for specified composition
std::string formula(const EmpiricalFormulaMap &elementCounts, bool richText = false);

// Return empirical formula for range
template <class Range, class Lam> std::string formula(const Range &range, Lam lambda, bool richText = false)
{
    EmpiricalFormulaMap map;

    for (const auto &obj : range)
        ++map[lambda(obj)];

    return formula(map, richText);
}
}; // namespace EmpiricalFormula
