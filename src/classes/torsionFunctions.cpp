// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "classes/speciesTorsion.h"
#include <map>

// Return enum options for TorsionFunction
EnumOptions<TorsionFunctions::Form> TorsionFunctions::forms()
{
    return EnumOptions<TorsionFunctions::Form>("TorsionFunction",
                                               {{TorsionFunctions::Form::None, "None"},
                                                {TorsionFunctions::Form::Cosine, "Cos", 4},
                                                {TorsionFunctions::Form::Cos3, "Cos3", 3},
                                                {TorsionFunctions::Form::Cos3C, "Cos3C", 4},
                                                {TorsionFunctions::Form::Cos4, "Cos4", 4},
                                                {TorsionFunctions::Form::CosN, "CosN", 1, OptionArguments::AnyNumber},
                                                {TorsionFunctions::Form::CosNC, "CosNC", 1, OptionArguments::AnyNumber},
                                                {TorsionFunctions::Form::UFFCosine, "UFFCosine", 3},
                                                {TorsionFunctions::Form::FourierN, "FourierN", 2, OptionArguments::AnyNumber}});
}

// Return parameters for specified form
const std::vector<std::string> &TorsionFunctions::parameters(Form form)
{
    static std::map<TorsionFunctions::Form, std::vector<std::string>> params_ = {
        {TorsionFunctions::Form::None, {}},
        {TorsionFunctions::Form::Cosine, {"k", "n", "eq", "s"}},
        {TorsionFunctions::Form::Cos3, {"k1", "k2", "k3"}},
        {TorsionFunctions::Form::Cos3C, {"k0", "k1", "k2", "k3"}},
        {TorsionFunctions::Form::Cos4, {"k1", "k2", "k3", "k4"}},
        {TorsionFunctions::Form::UFFCosine, {"k", "n", "eq"}},
        {TorsionFunctions::Form::CosN, {}},
        {TorsionFunctions::Form::CosNC, {}},
        {TorsionFunctions::Form::FourierN, {}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string TorsionFunctions::parameter(Form form, int n)
{
    if (form == TorsionFunctions::Form::CosN)
        return fmt::format("k{}", n + 1);
    else if (form == TorsionFunctions::Form::CosNC)
        return fmt::format("k{}", n);
    else if (form == TorsionFunctions::Form::FourierN)
        return n == 0 ? "k" : fmt::format("C{}", n);
    else
        return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

std::optional<std::pair<std::string_view, int>> parseVariableParameter(std::string_view name)
{
    auto alpha = DissolveSys::beforeChar(name, "0123456789");
    if (!alpha.empty())
    {
        bool isFP = true;
        if (DissolveSys::isNumber(name.substr(alpha.size()), isFP) && !isFP)
            return std::pair<std::string_view, int>(alpha, std::stoi(std::string(name.substr(alpha.size()))));
    }
    return {};
}

// Return index of parameter in the given form
std::optional<int> TorsionFunctions::parameterIndex(Form form, std::string_view name)
{
    if (form == TorsionFunctions::Form::CosN)
    {
        auto ai = parseVariableParameter(name);
        if (ai && ai.value().first == "k" && ai.value().second > 0)
            return ai.value().second - 1;
        else
            return {};
    }
    else if (form == TorsionFunctions::Form::CosNC)
    {
        auto ai = parseVariableParameter(name);
        if (ai && ai.value().first == "k" && ai.value().second >= 0)
            return ai.value().second;
        else
            return {};
    }
    else if (form == TorsionFunctions::Form::FourierN)
    {
        if (name == "k")
            return 0;
        else
        {
            auto ai = parseVariableParameter(name);
            if (ai && ai.value().first == "C" && ai.value().second >= 0)
                return ai.value().second;
            else
                return {};
        }
    }
    else
    {
        auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                               [name](const auto &param) { return DissolveSys::sameString(name, param); });
        if (it == parameters(form).end())
            return {};

        return it - parameters(form).begin();
    }
}
