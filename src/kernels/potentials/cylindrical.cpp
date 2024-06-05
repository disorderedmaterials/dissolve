// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/cylindrical.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"
#include "keywords/vec3Labels.h"
#include "types.h"

CylindricalPotential::CylindricalPotential(const InteractionPotential<Functions1D> &interactionPotential,
                                           const Vec3<double> &origin, const Vec3<double> &vector)
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Cylindrical), origin_(origin), vector_(vector)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Form", "Functional form and parameters for the potential",
                                                            interactionPotential_);
    keywords_.add<Vec3DoubleKeyword>("Vector", "Direction vector", vector_, Vec3Labels::LabelType::XYZLabels);

    setPotential(interactionPotential);
}

// Create and return a copy of this potential
std::unique_ptr<ExternalPotential> CylindricalPotential::duplicate() const
{
    return std::make_unique<CylindricalPotential>(interactionPotential_, origin_, vector_);
}

/*
 * Definition
 */

// Set potential form
void CylindricalPotential::setPotential(const InteractionPotential<Functions1D> &potential)
{
    interactionPotential_ = potential;
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Set coordinate origin of potential
void CylindricalPotential::setOrigin(const Vec3<double> &origin) { origin_ = origin; }

// Set vector of potential
void CylindricalPotential::setVector(const Vec3<double> &vector) { vector_ = vector; }

// Return functional form of the potential, as a string
const std::string CylindricalPotential::formString() const
{
    return Functions1D::forms().keyword(interactionPotential_.form());
}

// Return parameters of the potential, as a string
const std::string CylindricalPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double CylindricalPotential::energy(const Atom &i, const Box *box) const
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

    // Return energy at minimum distance between the atom and a point on the line
    return potentialFunction_.y(v.magnitude());
}

// Calculate force on specified atom, summing in to supplied vector
void CylindricalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
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

    // Get force at minimum distance between the atom and a point on the line
    f = v * -potentialFunction_.dYdX(v.magnitude());
}
