// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/restraint.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3Double.h"

RestraintPotential::RestraintPotential()
    : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Spherical),
      interactionPotential_(Functions1D::Form::Harmonic)
{
    keywords_.add<Vec3DoubleKeyword>("Origin", "Reference origin point", origin_, Vec3Labels::LabelType::XYZLabels);
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Form", "Functional form and parameters for the potential",
                                                            interactionPotential_);
}

/*
 * Definition
 */

// Set potential form
void RestraintPotential::setPotential(const InteractionPotential<Functions1D> &potential)
{
    interactionPotential_ = potential;
    potentialFunction_.setFormAndParameters(interactionPotential_.form(), interactionPotential_.parameters());
}

// Set coordinate origin of potential
void RestraintPotential::setOrigin(const Vec3<double> &origin) { origin_ = origin; }

// Return functional form of the potential, as a string
const std::string RestraintPotential::formString() const { return Functions1D::forms().keyword(interactionPotential_.form()); }

// Return parameters of the potential, as a string
const std::string RestraintPotential::formParametersString() const { return interactionPotential_.parametersAsString(); }

/*
 * Potential Calculation
 */

// Calculate energy on specified atom
double RestraintPotential::energy(const Atom &i, const Box *box) const
{
    return potentialFunction_.y(box->minimumDistanceSquared(i.r(), origin_));
}

// Calculate force on specified atom, summing in to supplied vector
void RestraintPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    // Get normalised vector and distance
    auto v = box->minimumVector(i.r(), origin_);
    auto r = v.magAndNormalise();

    f = v * -potentialFunction_.dYdX(r);
}
