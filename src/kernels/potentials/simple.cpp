// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/simple.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionpotential.h"
#include "keywords/vec3double.h"

// Return enum options for SimplePotentialFunctions
EnumOptions<SimplePotentialFunctions::Form> SimplePotentialFunctions::forms()
{
    return EnumOptions<SimplePotentialFunctions::Form>("SimplePotentialFunction",
                                                       {{SimplePotentialFunctions::Form::Harmonic, "Harmonic", 1},
                                                        {SimplePotentialFunctions::Form::SoftSphere, "SoftSphere", 3},
                                                        {SimplePotentialFunctions::Form::LJSphere, "LJSphere", 2}});
}

// Return parameters for specified form
const std::vector<std::string> &SimplePotentialFunctions::parameters(Form form)
{
    static std::map<SimplePotentialFunctions::Form, std::vector<std::string>> params_ = {
        {SimplePotentialFunctions::Form::Harmonic, {"k"}},
        {SimplePotentialFunctions::Form::SoftSphere, {"epsilon", "sigma", "N"}},
        {SimplePotentialFunctions::Form::LJSphere, {"epsilon", "sigma"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string SimplePotentialFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> SimplePotentialFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

SimplePotential::SimplePotential()
    : interactionPotential_(SimplePotentialFunctions::Form::Harmonic),
      ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Spherical)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<SimplePotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
}

/*
 * Definition
 */

// Set potential form
void SimplePotential::setPotential(const InteractionPotential<SimplePotentialFunctions> &potential)
{
    interactionPotential_ = potential;
}

// Set coordinate origin of potential
void SimplePotential::setOrigin(Vec3<double> origin) { origin_ = origin; }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double SimplePotential::energy(const Atom &i, const Box *box) const
{
    switch (interactionPotential_.form())
    {
        case (SimplePotentialFunctions::Form::Harmonic):
            return 0.5 * interactionPotential_.parameters()[0] * box->minimumDistanceSquared(i.r(), origin_);
        case (SimplePotentialFunctions::Form::SoftSphere):
            return interactionPotential_.parameters()[0] *
                   pow(interactionPotential_.parameters()[1] / box->minimumDistance(i.r(), origin_),
                       interactionPotential_.parameters()[2]);
        case (SimplePotentialFunctions::Form::LJSphere):
        {
            auto r = box->minimumDistance(i.r(), origin_);
            auto epsilon = interactionPotential_.parameters()[0];
            auto sigma = interactionPotential_.parameters()[1];
            return 4.0 * epsilon * (pow(sigma / r, 12) - pow(sigma / r, 6));
        }
        default:
            throw(std::runtime_error(fmt::format("Requested functional form of SimplePotential has not been implemented.\n")));
    }
}

// Calculate force on specified atom, summing in to supplied vector
void SimplePotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto vecji = box->minimumVector(i.r(), origin_);
    auto r = vecji.magAndNormalise();

    // Calculate final force multiplier
    auto forceMultiplier = 0.0;
    switch (interactionPotential_.form())
    {
        case (SimplePotentialFunctions::Form::Harmonic):
            forceMultiplier = -interactionPotential_.parameters()[0] * r;
            break;
        case (SimplePotentialFunctions::Form::LJSphere):
        {
            auto epsilon = interactionPotential_.parameters()[0];
            auto sigma = interactionPotential_.parameters()[1];
            forceMultiplier = -48.0 * epsilon * (pow(r, 12) - 0.5 * pow(r, 6)) / (r * sigma);
            break;
        }
            throw(std::runtime_error(fmt::format("Requested functional form of SimplePotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= vecji * forceMultiplier;
}
