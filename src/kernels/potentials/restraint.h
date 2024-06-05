// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/base.h"

// Simple Restraint Potential
class RestraintPotential : public ExternalPotential
{
    public:
    RestraintPotential();
    ~RestraintPotential() = default;

    /*
     * Definition
     */
    private:
    // Potential form
    InteractionPotential<Functions1D> interactionPotential_;
    Function1DWrapper potentialFunction_;
    // Coordinate origin of potential
    Vec3<double> origin_;

    public:
    // Set potential form
    void setPotential(const InteractionPotential<Functions1D> &potential);
    // Set coordinate origin of potential
    void setOrigin(const Vec3<double> &origin);
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
