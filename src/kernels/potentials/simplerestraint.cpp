// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/potentials/simplerestraint.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionpotential.h"
#include "keywords/vec3double.h"

// Return enum options for SimpleRestraintPotentialFunctions
EnumOptions<SimpleRestraintPotentialFunctions::Form> SimpleRestraintPotentialFunctions::forms()
{
    return EnumOptions<SimpleRestraintPotentialFunctions::Form>(
        "SimpleRestraintPotentialFunction", {{SimpleRestraintPotentialFunctions::Form::Harmonic, "Harmonic", 1}});
}

// Return parameters for specified form
const std::vector<std::string> &SimpleRestraintPotentialFunctions::parameters(Form form)
{
    static std::map<SimpleRestraintPotentialFunctions::Form, std::vector<std::string>> params_ = {
        {SimpleRestraintPotentialFunctions::Form::Harmonic, {"k"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string SimpleRestraintPotentialFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> SimpleRestraintPotentialFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

SimpleRestraintPotential::SimpleRestraintPotential()
    : interactionPotential_(SimpleRestraintPotentialFunctions::Form::Harmonic),
      ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Spherical)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<SimpleRestraintPotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
}

/*
 * Definition
 */

// Set potential form
void SimpleRestraintPotential::setPotential(const InteractionPotential<SimpleRestraintPotentialFunctions> &potential)
{
    interactionPotential_ = potential;
}

// Set coordinate origin of potential
void SimpleRestraintPotential::setOrigin(Vec3<double> origin) { origin_ = origin; }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double SimpleRestraintPotential::energy(const Atom &i, const Box *box) const
{
    switch (interactionPotential_.form())
    {
        case (SimpleRestraintPotentialFunctions::Form::Harmonic):
            return 0.5 * interactionPotential_.parameters()[0] * box->minimumDistanceSquared(i.r(), origin_);
        default:
            throw(std::runtime_error(
                fmt::format("Requested functional form of SimpleRestraintPotential has not been implemented.\n")));
    }
}

// Calculate force on specified atom, summing in to supplied vector
void SimpleRestraintPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto vecji = box->minimumVector(i.r(), origin_);
    auto r = vecji.magAndNormalise();

    // Calculate final force multiplier
    auto forceMultiplier = 0.0;
    switch (interactionPotential_.form())
    {
        case (SimpleRestraintPotentialFunctions::Form::Harmonic):
            forceMultiplier = -interactionPotential_.parameters()[0] * r;
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Requested functional form of SimpleRestraintPotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= vecji * forceMultiplier;
}
