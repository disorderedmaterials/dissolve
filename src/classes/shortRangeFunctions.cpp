// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/shortRangeFunctions.h"

// Return enum options for ShortRangeType
EnumOptions<ShortRangeFunctions::Form> ShortRangeFunctions::forms()
{
    return EnumOptions<ShortRangeFunctions::Form>("ShortRangeType",
                                                  {{ShortRangeFunctions::Form::Undefined, "Undefined"},
                                                   {ShortRangeFunctions::Form::LennardJones, "LJ", 2, 2},
                                                   {ShortRangeFunctions::Form::LennardJonesGeometric, "LJGeometric", 2, 2},
                                                   {ShortRangeFunctions::Form::Buckingham, "Buckingham", 3, 3}});
}

// Return parameters for specified form
const std::vector<std::string> &ShortRangeFunctions::parameters(Form form)
{
    static std::map<ShortRangeFunctions::Form, std::vector<std::string>> params_ = {
        {ShortRangeFunctions::Form::Undefined, {}},
        {ShortRangeFunctions::Form::LennardJones, {"epsilon", "sigma"}},
        {ShortRangeFunctions::Form::LennardJonesGeometric, {"epsilon", "sigma"}},
        {ShortRangeFunctions::Form::Buckingham, {"A", "B", "C"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string ShortRangeFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> ShortRangeFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}

/*
 * Utility
 */

// Combine parameters for the two atom types using suitable rules
std::optional<InteractionPotential<Functions1D>>
ShortRangeFunctions::combine(const InteractionPotential<ShortRangeFunctions> &srI,
                             const InteractionPotential<ShortRangeFunctions> &srJ)
{
    if (srI.form() == Form::Undefined || srJ.form() == Form::Undefined)
    {
        Messenger::error("Can't combine parameters since one or both short range functions are undefined.\n");
        return {};
    }

    // Equivalent functional forms?
    if (srI.form() == srJ.form())
    {
        switch (srI.form())
        {
            case (Form::LennardJones):
                /*
                 * Combine parameters (Lorentz-Berthelot):
                 * Parameter 0 = Epsilon
                 * Parameter 1 = Sigma
                 */
                return InteractionPotential<Functions1D>(
                    Functions1D::Form::LennardJones126,
                    {sqrt(srI.parameters()[0] * srJ.parameters()[0]), (srI.parameters()[1] + srJ.parameters()[1]) * 0.5});
            case (Form::LennardJonesGeometric):
                /*
                 * Combine parameters (Geometric):
                 * Parameter 0 = Epsilon
                 * Parameter 1 = Sigma
                 */
                return InteractionPotential<Functions1D>(
                    Functions1D::Form::LennardJones126,
                    {sqrt(srI.parameters()[0] * srJ.parameters()[0]), sqrt(srI.parameters()[1] * srJ.parameters()[1])});
            default:
                Messenger::error("Short-range type {} is not combinable.\n", forms().keyword(srI.form()));
                return {};
        }
    }
    else
    {
        // In the case of combining LJ and LJGeometric, default to standard Lorentz-Berthelot rules
        auto ljI = srI.form() == ShortRangeFunctions::Form::LennardJones ||
                   srI.form() == ShortRangeFunctions::Form::LennardJonesGeometric;
        auto ljJ = srJ.form() == ShortRangeFunctions::Form::LennardJones ||
                   srJ.form() == ShortRangeFunctions::Form::LennardJonesGeometric;
        if (ljI && ljJ)
        {
            Messenger::warn("Defaulting to Lorentz-Berthelot rules to combine parameters...\n");

            /*
             * Combine parameters (Lorentz-Berthelot):
             * Parameter 0 = Epsilon
             * Parameter 1 = Sigma
             */
            return InteractionPotential<Functions1D>(
                Functions1D::Form::LennardJones126,
                {sqrt(srI.parameters()[0] * srJ.parameters()[0]), (srI.parameters()[1] + srJ.parameters()[1]) * 0.5});
        }
        else
            Messenger::error("Can't combine parameters between dislike ShortRangeFunctions {} and {}.\n",
                             forms().keyword(srI.form()), forms().keyword(srJ.form()));
    }

    return {};
}
