// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/empiricalFormula.h"

// Return empirical formula for specified composition
std::string EmpiricalFormula::formula(const EmpiricalFormulaMap &elementCounts, bool richText)
{
    std::string empiricalFormula;

    for (auto it = elementCounts.crbegin(); it != elementCounts.crend(); ++it)
    {
        auto &[Z, count] = *it;
        if (count == 0)
        {
            continue;
        }
        else if (count == 1)
        {
            empiricalFormula += std::format("{}", Elements::symbol(Z));
        }
        else
        {
            if (richText)
                empiricalFormula += std::format("{}<sub>{}</sub>", Elements::symbol(Z), count);
            else
                empiricalFormula += std::format("{}{}", Elements::symbol(Z), count);
        }
    }

    return empiricalFormula;
}
