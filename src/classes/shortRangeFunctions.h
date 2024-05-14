// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/interactionPotential.h"
#include "math/function1D.h"
#include <optional>

// Short-range functional forms
class ShortRangeFunctions
{
    public:
    enum class Form
    {
        None,                 /* No short-range dispersive forces */
        LennardJones,         /* Lennard-Jones 12-6 form with Lorentz-Berthelot combination rules */
        LennardJonesGeometric /* Lennard-Jones 12-6 form with Geometric combination rules */
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);

    /*
     * Usage
     */
    public:
    // Combine parameters for the two atom types using suitable rules
    static InteractionPotential<Functions1D> combine(const InteractionPotential<ShortRangeFunctions> &srI,
                                                     const InteractionPotential<ShortRangeFunctions> &srJ);
};
