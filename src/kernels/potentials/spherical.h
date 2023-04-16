// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/interactionpotential.h"
#include "kernels/potentials/base.h"

// SphericalPotential functional forms
class SphericalPotentialFunctions
{
    public:
    enum class Form
    {
        Harmonic /* Harmonic well potential */
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};

// Spherical Potential
class SphericalPotential : public ExternalPotential
{
    public:
    SphericalPotential();
    ~SphericalPotential() = default;

    /*
     * Definition
     */

    private:
    // Potential form
    InteractionPotential<SphericalPotentialFunctions> interactionPotential_;
    // Coordinate origin of external potential
    Vec3<double> origin_;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    double energy(const Atom &i, const Box *box) const override;
    // Calculate force on specified atom, summing in to supplied vector
    void force(const Atom &i, const Box *box, Vec3<double> &f) const override;
};
