// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    DirectionalPotential(const InteractionPotential<DirectionalPotentialFunctions> &interactionPotential =
                             {DirectionalPotentialFunctions::Form::LJCylinder},
                         const Vec3<double> &origin = {0.0, 0.0, 0.0}, const Vec3<double> &vector = {0.0, 0.0, 1.0});
    ~DirectionalPotential() = default;
    // Create and return a copy of this potential
    std::unique_ptr<ExternalPotential> duplicate() const override;

    /*
     * Definition
     */
    private:
    // Potential form
    InteractionPotential<DirectionalPotentialFunctions> interactionPotential_;
    // Coordinate origin of potential
    Vec3<double> origin_;
    // Direction of potential
    Vec3<double> vector_;

    public:
    // Set potential form
    void setPotential(const InteractionPotential<DirectionalPotentialFunctions> &potential);
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
