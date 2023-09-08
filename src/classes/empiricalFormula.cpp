// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
            empiricalFormula += fmt::format("{}", Elements::symbol(Z));
        }
        else
        {
            if (richText)
                empiricalFormula += fmt::format("{}<sub>{}</sub>", Elements::symbol(Z), count);
            else
                empiricalFormula += fmt::format("{}{}", Elements::symbol(Z), count);
        }
    }

    return empiricalFormula;
}
