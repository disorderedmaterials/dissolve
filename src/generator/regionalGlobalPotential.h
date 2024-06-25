// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "kernels/potentials/regional.h"

// Regional Global Potential
class RegionalGlobalPotentialGeneratorNode : public RegionalPotentialVoxelKernel, public GeneratorNode
{
    public:
    RegionalGlobalPotentialGeneratorNode();
    ~RegionalGlobalPotentialGeneratorNode() override = default;

    private:
    // Guide voxel size (Angstroms)
    double voxelSize_{0.5};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
