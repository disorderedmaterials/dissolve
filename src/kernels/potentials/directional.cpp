// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/directional.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"
#include "keywords/vec3Labels.h"
#include "types.h"

// Return enum options for DirectionalPotentialFunctions
EnumOptions<DirectionalPotentialFunctions::Form> DirectionalPotentialFunctions::forms()
{
    return EnumOptions<DirectionalPotentialFunctions::Form>(
        "DirectionalPotentialFunction", {{DirectionalPotentialFunctions::Form::LJCylinder, "LJCylinder", 2}});
}

// Return parameters for specified form
const std::vector<std::string> &DirectionalPotentialFunctions::parameters(Form form)
{
    static std::map<DirectionalPotentialFunctions::Form, std::vector<std::string>> params_ = {
        {DirectionalPotentialFunctions::Form::LJCylinder, {"epsilon", "sigma"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string DirectionalPotentialFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> DirectionalPotentialFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

DirectionalPotential::DirectionalPotential()
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Directional),
      interactionPotential_(DirectionalPotentialFunctions::Form::LJCylinder)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<DirectionalPotentialFunctions>>(
        "Form", "Functional form and parameters for the potential", interactionPotential_);
    keywords_.add<Vec3DoubleKeyword>("Vector", "Direction vector", vector_, Vec3Labels::LabelType::XYZLabels);
}

/*
 * Definition
 */

// Set potential form
void DirectionalPotential::setPotential(const InteractionPotential<DirectionalPotentialFunctions> &potential)
{
    interactionPotential_ = potential;
}

// Set coordinate origin of potential
void DirectionalPotential::setOrigin(Vec3<double> origin) { origin_ = origin; }

// Set vector of potential
void DirectionalPotential::setVector(Vec3<double> vector) { vector_ = vector; }

// Return functional form of the potential, as a string
const std::string DirectionalPotential::formString() const
{
    return DirectionalPotentialFunctions::forms().keyword(interactionPotential_.form());
}

// Return parameters of the potential, as a string
const std::string DirectionalPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double DirectionalPotential::energy(const AtomRef i, const Box *box) const
{
    // Vector between the position of the atom and the origin
    auto v = box->minimumVector(i.r(), origin_);

    // See: A Programmers Geometry, Bowyer and Woodwark, Butterworths (pub.), 1983, p99

    // Cross product between the line and the position of the atom in the xy plane
    auto xyyx = vector_.x * v.y - vector_.y * v.x;
    // Cross product between the line and the position of the atom in the xz plane
    auto xzzx = vector_.x * v.z - vector_.z * v.x;
    // Cross product between the line and the position of the atom in the yz plane
    auto yzzy = vector_.y * v.z - vector_.z * v.y;

    // Compute new components
    v.x = vector_.y * xyyx + vector_.z * xzzx;
    v.y = vector_.z * yzzy - vector_.x * xyyx;
    v.z = -vector_.x * xzzx - vector_.y * yzzy;

    // Minimum distance between the atom and a point on the line
    auto r = v.magnitude();

    switch (interactionPotential_.form())
    {
        case (DirectionalPotentialFunctions::Form::LJCylinder):
        {
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6);
            auto sigmar12 = sigmar6 * sigmar6;
            return 4.0 * interactionPotential_.parameters()[0] * (sigmar12 - sigmar6);
        }
        default:
            throw(std::runtime_error(
                fmt::format("Requested functional form of DirectionalPotential has not been implemented.\n")));
    }
}

// Calculate force on specified atom, summing in to supplied vector
void DirectionalPotential::force(const AtomRef i, const Box *box, Vec3<double> &f) const
{
    // Vector between the position of the atom and the origin
    auto v = box->minimumVector(i.r(), origin_);

    // See: A Programmers Geometry, Bowyer and Woodwark, Butterworths (pub.), 1983, p99

    // Cross product between the line and the position of the atom in the xy plane
    auto xyyx = vector_.x * v.y - vector_.y * v.x;
    // Cross product between the line and the position of the atom in the xz plane
    auto xzzx = vector_.x * v.z - vector_.z * v.x;
    // Cross product between the line and the position of the atom in the yz plane
    auto yzzy = vector_.y * v.z - vector_.z * v.y;

    // Compute new components
    v.x = vector_.y * xyyx + vector_.z * xzzx;
    v.y = vector_.z * yzzy - vector_.x * xyyx;
    v.z = -vector_.x * xzzx - vector_.y * yzzy;

    // Minimum distance between the atom and a point on the line
    auto r = v.magnitude();

    // Calculate final force multiplier
    auto forceMultiplier = 0.0;
    switch (interactionPotential_.form())
    {
        case (DirectionalPotentialFunctions::Form::LJCylinder):
        {
            auto sigmar = interactionPotential_.parameters()[1] / r;
            auto sigmar6 = pow(sigmar, 6.0);
            forceMultiplier = -48.0 * interactionPotential_.parameters()[0] * sigmar6 * (-sigmar6 + 0.5) / r;
            break;
        }
        default:
            throw(std::runtime_error(
                fmt::format("Requested functional form of DirectionalPotential has not been implemented.\n")));
    }

    // Sum in forces on the atom
    f -= v * forceMultiplier;
}
