// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "kernels/potentials/cylindrical.h"

// Cylindrical Global Potential
class CylindricalGlobalPotentialGeneratorNode : public GeneratorNode
{
    public:
    CylindricalGlobalPotentialGeneratorNode();
    ~CylindricalGlobalPotentialGeneratorNode() override = default;

    /*
     * Potential Function
     */
    private:
    // Potential form
    InteractionPotential<Functions1D> potential_;
    // Origin coordinates
    Vec3<NodeValue> origin_;
    bool originIsFractional_{false};
    // Cylindrical vector
    Vec3<NodeValue> vector_{0.0, 0.0, 1.0};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
