// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/base.h"

// Spherical Potential
class SphericalPotential : public ExternalPotential
{
    public:
    SphericalPotential(const InteractionPotential<Functions1D> &interactionPotential = {Functions1D::Form::Harmonic},
                       const Vector3 &origin = {0.0, 0.0, 0.0});
    ~SphericalPotential() = default;
    // Create and return a copy of this potential
    std::unique_ptr<ExternalPotential> duplicate() const override;

    /*
     * Definition
     */

    private:
    // Potential form
    InteractionPotential<Functions1D> interactionPotential_;
    Function1DWrapper potentialFunction_;
    // Coordinate origin of potential
    Vector3 origin_;

    public:
    // Set potential form
    void setPotential(const InteractionPotential<Functions1D> &potential);
    // Set coordinate origin of potential
    void setOrigin(const Vector3 &origin);
    // Return functional form of the potential, as a string
    const std::string formString() const override;
    // Return parameters of the potential, as a string
    const std::string formParametersString() const override;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    double energy(const ConfigurationAtom &i, const Box &box) const override;
    // Calculate force on specified atom, summing in to supplied vector
    void force(const ConfigurationAtom &i, const Box &box, Vector3 &f) const override;
};
