// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesAngle.h"

// Return enum options for AngleFunction
EnumOptions<AngleFunctions::Form> AngleFunctions::forms()
{
    return EnumOptions<AngleFunctions::Form>("AngleFunction", {{AngleFunctions::Form::None, "None"},
                                                               {AngleFunctions::Form::Harmonic, "Harmonic", 2},
                                                               {AngleFunctions::Form::Cosine, "Cos", 4},
                                                               {AngleFunctions::Form::Cos2, "Cos2", 4}});
}

// Return parameters for specified form
const std::vector<std::string> &AngleFunctions::parameters(Form form)
{
    static std::map<AngleFunctions::Form, std::vector<std::string>> params_ = {
        {AngleFunctions::Form::None, {}},
        {AngleFunctions::Form::Harmonic, {"k", "eq"}},
        {AngleFunctions::Form::Cosine, {"k", "n", "eq", "s"}},
        {AngleFunctions::Form::Cos2, {"k", "C0", "C1", "C2"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string AngleFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> AngleFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}
