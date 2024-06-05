// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/spherical.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"

// Return enum options for SphericalPotentialFunctions
EnumOptions<SphericalPotentialFunctions::Form> SphericalPotentialFunctions::forms()
{
    return EnumOptions<SphericalPotentialFunctions::Form>(
        "SphericalPotentialFunction",
        {{SphericalPotentialFunctions::Form::Harmonic, "Harmonic", 1}, {SphericalPotentialFunctions::Form::LJ, "LJ", 2}});
}

// Return parameters for specified form
const std::vector<std::string> &SphericalPotentialFunctions::parameters(Form form)
{
    static std::map<SphericalPotentialFunctions::Form, std::vector<std::string>> params_ = {
        {SphericalPotentialFunctions::Form::Harmonic, {"k"}}, {SphericalPotentialFunctions::Form::LJ, {"epsilon", "sigma"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string SphericalPotentialFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> SphericalPotentialFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

SphericalPotential::SphericalPotential(const InteractionPotential<SphericalPotentialFunctions> &interactionPotential,
                                       const Vec3<double> &origin)
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Spherical), interactionPotential_(interactionPotential),
      origin_(origin)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<SphericalPotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
}

// Create and return a copy of this potential
std::unique_ptr<ExternalPotential> SphericalPotential::duplicate() const
{
    return std::make_unique<SphericalPotential>(interactionPotential_, origin_);
}

/*
 * Definition
 */

// Set potential form
void SphericalPotential::setPotential(const InteractionPotential<SphericalPotentialFunctions> &potential)
{
    interactionPotential_ = potential;
}

// Set coordinate origin of potential
void SphericalPotential::setOrigin(Vec3<double> origin) { origin_ = origin; }

// Return functional form of the potential, as a string
const std::string SphericalPotential::formString() const
{
    return SphericalPotentialFunctions::forms().keyword(interactionPotential_.form());
}

// Return parameters of the potential, as a string
const std::string SphericalPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double SphericalPotential::energy(const Atom &i, const Box *box) const
{
    switch (interactionPotential_.form())
    {
        case (SphericalPotentialFunctions::Form::Harmonic):
            return 0.5 * interactionPotential_.parameters()[0] * box->minimumDistanceSquared(i.r(), origin_);
        case (SphericalPotentialFunctions::Form::LJ):
        {
            auto r = box->minimumDistance(i.r(), origin_);
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6);
            auto sigmar12 = sigmar6 * sigmar6;
            return 4.0 * interactionPotential_.parameters()[0] * (sigmar12 - sigmar6);
        }
        default:
            throw(
                std::runtime_error(fmt::format("Requested functional form of SphericalPotential has not been implemented.\n")));
    }
}

// Calculate force on specified atom, summing in to supplied vector
void SphericalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto vecji = box->minimumVector(i.r(), origin_);
    auto r = vecji.magAndNormalise();

    // Calculate final force multiplier
    auto forceMultiplier = 0.0;
    switch (interactionPotential_.form())
    {
        case (SphericalPotentialFunctions::Form::Harmonic):
            forceMultiplier = -interactionPotential_.parameters()[0] * r;
            break;
        case (SphericalPotentialFunctions::Form::LJ):
        {
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6.0);
            forceMultiplier = -48.0 * interactionPotential_.parameters()[0] * sigmar6 * (-sigmar6 + 0.5) / r;
            break;
        }
        default:
            throw(
                std::runtime_error(fmt::format("Requested functional form of SphericalPotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= vecji * forceMultiplier;
}
