// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/base.h"

// Cylindrical Potential
class CylindricalPotential : public ExternalPotential
{
    public:
    CylindricalPotential(const InteractionPotential<Functions1D> &interactionPotential = {Functions1D::Form::LennardJones126},
                         const Vec3<double> &origin = {0.0, 0.0, 0.0}, const Vec3<double> &vector = {0.0, 0.0, 1.0});
    ~CylindricalPotential() = default;
    // Create and return a copy of this potential
    std::unique_ptr<ExternalPotential> duplicate() const override;

    /*
     * Definition
     */
    private:
    // Potential function
    InteractionPotential<Functions1D> interactionPotential_;
    Function1DWrapper potentialFunction_;
    // Coordinate origin of potential
    Vec3<double> origin_;
    // Direction of potential
    Vec3<double> vector_;

    public:
    // Set potential form
    void setPotential(const InteractionPotential<Functions1D> &potential);
    // Set coordinate origin of potential
    void setOrigin(Vec3<double> origin);
    // Set vector of potential
    void setVector(Vec3<double> vector);
    // Return functional form of the potential, as a string
    const std::string formString() const override;
    // Return parameters of the potential, as a string
    const std::string formParametersString() const override;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    double energy(const Atom &i, const Box *box) const override;
    // Calculate force on specified atom, summing in to supplied vector
    void force(const Atom &i, const Box *box, Vec3<double> &f) const override;
};
