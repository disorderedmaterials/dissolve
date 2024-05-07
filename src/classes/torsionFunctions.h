// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"

// Torsion functional forms
class TorsionFunctions
{
    public:
    enum class Form
    {
        None,
        Cosine,
        Cos3,
        Cos3C,
        Cos4,
        CosN,
        CosNC,
        UFFCosine,
        FourierN
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};
