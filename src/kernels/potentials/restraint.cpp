// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/restraint.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"

// Return enum options for RestraintPotentialFunctions
EnumOptions<RestraintPotentialFunctions::Form> RestraintPotentialFunctions::forms()
{
    return EnumOptions<RestraintPotentialFunctions::Form>("RestraintPotentialFunction",
                                                          {{RestraintPotentialFunctions::Form::Harmonic, "Harmonic", 1}});
}

// Return parameters for specified form
const std::vector<std::string> &RestraintPotentialFunctions::parameters(Form form)
{
    static std::map<RestraintPotentialFunctions::Form, std::vector<std::string>> params_ = {
        {RestraintPotentialFunctions::Form::Harmonic, {"k"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string RestraintPotentialFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> RestraintPotentialFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

RestraintPotential::RestraintPotential()
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Simple),
      interactionPotential_(RestraintPotentialFunctions::Form::Harmonic)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<RestraintPotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
}

/*
 * Definition
 */

// Set potential form
void RestraintPotential::setPotential(const InteractionPotential<RestraintPotentialFunctions> &potential)
{
    interactionPotential_ = potential;
}

// Set coordinate origin of potential
void RestraintPotential::setOrigin(Vec3<double> origin) { origin_ = origin; }

// Return functional form of the potential, as a string
const std::string RestraintPotential::formString() const
{
    return RestraintPotentialFunctions::forms().keyword(interactionPotential_.form());
}

// Return parameters of the potential, as a string
const std::string RestraintPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double RestraintPotential::energy(const Atom &i, const Box *box) const
{
    switch (interactionPotential_.form())
    {
        case (RestraintPotentialFunctions::Form::Harmonic):
            return 0.5 * interactionPotential_.parameters()[0] * box->minimumDistanceSquared(i.r(), origin_);
        default:
            throw(
                std::runtime_error(fmt::format("Requested functional form of RestraintPotential has not been implemented.\n")));
    }
}

// Calculate force on specified atom, summing in to supplied vector
void RestraintPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto vecji = box->minimumVector(i.r(), origin_);
    auto r = vecji.magAndNormalise();

    // Calculate final force multiplier
    auto forceMultiplier = 0.0;
    switch (interactionPotential_.form())
    {
        case (RestraintPotentialFunctions::Form::Harmonic):
            forceMultiplier = -interactionPotential_.parameters()[0] * r;
            break;
        default:
            throw(
                std::runtime_error(fmt::format("Requested functional form of RestraintPotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= vecji * forceMultiplier;
}
