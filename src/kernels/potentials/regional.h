// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "classes/region.h"
#include "kernels/potentials/base.h"

// Regional Potential
class RegionalPotential : public ExternalPotential
{
    public:
    RegionalPotential();
    ~RegionalPotential() = default;

    /*
     * Definition
     */
    private:
    // Local variables, set when checking voxels
    std::shared_ptr<ExpressionVariable> x_, y_, z_, xFrac_, yFrac_, zFrac_;
    // Expression describing region
    NodeValue expression_;
    // Minimum threshold value for function
    double minimumValue_{0.0};
    // Maximum threshold value for function
    double maximumValue_{1.0};
    // Guide voxel size (Angstroms)
    double voxelSize_{1.0};
    // Fractional voxel size
    Vec3<double> voxelSizeFrac_;
    // Vector fields for energy and derived force
    Array3D<double> energyVoxels_;
    Array3D<Vec3<double>> forceVoxels_;

    private:
    // Return voxel coordinates of supplied atom
    std::tuple<int, int, int> voxelIndices(const Atom &i, const Box *box) const;

    public:
    // Set up potential for supplied box
    bool setUp(const Box *box);
    // Set expression
    //    void setPotential(const InteractionPotential<RegionalPotentialFunctions> &potential);
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
