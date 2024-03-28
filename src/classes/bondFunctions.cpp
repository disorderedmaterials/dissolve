// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesBond.h"

// Return enum options for BondFunction
EnumOptions<BondFunctions::Form> BondFunctions::forms()
{
    return EnumOptions<BondFunctions::Form>("BondFunction", {{BondFunctions::Form::None, "None"},
                                                             {BondFunctions::Form::Harmonic, "Harmonic", 2},
                                                             {BondFunctions::Form::EPSR, "EPSR", 2},
                                                             {BondFunctions::Form::Morse, "Morse", 3}});
}

// Return parameters for specified form
const std::vector<std::string> &BondFunctions::parameters(Form form)
{
    static std::map<BondFunctions::Form, std::vector<std::string>> params_ = {
        {BondFunctions::Form::None, {}},
        {BondFunctions::Form::Harmonic, {"k", "eq"}},
        {BondFunctions::Form::EPSR, {"C/2", "eq"}},
        {BondFunctions::Form::Morse, {"D", "alpha", "eq"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string BondFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> BondFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}
