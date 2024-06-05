// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/spherical.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"

SphericalPotential::SphericalPotential(const InteractionPotential<Functions1D> &interactionPotential,
                                       const Vec3<double> &origin)
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Spherical), interactionPotential_(interactionPotential),
      origin_(origin)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Form", "Functional form and parameters for the potential",
                                                            interactionPotential_);
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
void SphericalPotential::setPotential(const InteractionPotential<Functions1D> &potential)
{
    interactionPotential_ = potential;
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Set coordinate origin of potential
void SphericalPotential::setOrigin(const Vec3<double> &origin) { origin_ = origin; }

// Return functional form of the potential, as a string
const std::string SphericalPotential::formString() const { return Functions1D::forms().keyword(interactionPotential_.form()); }

// Return parameters of the potential, as a string
const std::string SphericalPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double SphericalPotential::energy(const Atom &i, const Box *box) const
{
    return potentialFunction_.y(box->minimumDistanceSquared(i.r(), origin_));
}

// Calculate force on specified atom, summing in to supplied vector
void SphericalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto v = box->minimumVector(i.r(), origin_);
    auto r = v.magAndNormalise();

    f = v * -potentialFunction_.dYdX(r);
}
