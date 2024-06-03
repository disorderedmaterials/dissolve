// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/simple.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"

// Return enum options for SimplePotentialFunctions
EnumOptions<SimplePotentialFunctions::Form> SimplePotentialFunctions::forms()
{
    return EnumOptions<SimplePotentialFunctions::Form>(
        "SimplePotentialFunction",
        {{SimplePotentialFunctions::Form::Harmonic, "Harmonic", 1}, {SimplePotentialFunctions::Form::LJ, "LJ", 2}});
}

// Return parameters for specified form
const std::vector<std::string> &SimplePotentialFunctions::parameters(Form form)
{
    static std::map<SimplePotentialFunctions::Form, std::vector<std::string>> params_ = {
        {SimplePotentialFunctions::Form::Harmonic, {"k"}}, {SimplePotentialFunctions::Form::LJ, {"epsilon", "sigma"}}};
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

SimplePotential::SimplePotential(const InteractionPotential<SimplePotentialFunctions> &interactionPotential,
                                 const Vec3<double> &origin)
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Simple), interactionPotential_(interactionPotential),
      origin_(origin)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<SimplePotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
}

// Create and return a copy of this potential
std::unique_ptr<ExternalPotential> SimplePotential::duplicate() const
{
    return std::make_unique<SimplePotential>(interactionPotential_, origin_);
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

// Return functional form of the potential, as a string
const std::string SimplePotential::formString() const
{
    return SimplePotentialFunctions::forms().keyword(interactionPotential_.form());
}

// Return parameters of the potential, as a string
const std::string SimplePotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

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
        case (SimplePotentialFunctions::Form::LJ):
        {
            auto r = box->minimumDistance(i.r(), origin_);
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6);
            auto sigmar12 = sigmar6 * sigmar6;
            return 4.0 * interactionPotential_.parameters()[0] * (sigmar12 - sigmar6);
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
        case (SimplePotentialFunctions::Form::LJ):
        {
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6.0);
            forceMultiplier = -48.0 * interactionPotential_.parameters()[0] * sigmar6 * (-sigmar6 + 0.5) / r;
            break;
        }
        default:
            throw(std::runtime_error(fmt::format("Requested functional form of SimplePotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= vecji * forceMultiplier;
}
