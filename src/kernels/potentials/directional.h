// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/base.h"

// DirectionalPotential functional forms
class DirectionalPotentialFunctions
{
    public:
    enum class Form
    {
        LJCylinder /* Lennard-Jones cylindrical potential */
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

// Directional Potential
class DirectionalPotential : public ExternalPotential
{
    public:
    DirectionalPotential();
    ~DirectionalPotential() = default;

    /*
     * Definition
     */

    private:
    // Potential form
    InteractionPotential<DirectionalPotentialFunctions> interactionPotential_;
    // Coordinate origin of potential
    Vec3<double> origin_;
    // Direction of potential
    Vec3<double> vector_{0.0, 0.0, 1.0};

    public:
    // Set potential form
    void setPotential(const InteractionPotential<DirectionalPotentialFunctions> &potential);
    // Set coordinate origin of potential
    void setOrigin(Vec3<double> origin);
    // Set vector of potential
    void setVector(Vec3<double> vector);

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    double energy(const Atom &i, const Box *box) const override;
    // Calculate force on specified atom, summing in to supplied vector
    void force(const Atom &i, const Box *box, Vec3<double> &f) const override;
};
