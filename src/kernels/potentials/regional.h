// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "classes/region.h"
#include "kernels/potentials/base.h"

// Regional Potential Voxel Kernel
class RegionalPotentialVoxelKernel
{
    public:
    explicit RegionalPotentialVoxelKernel(std::string_view expressionString = "",
                                          std::vector<std::shared_ptr<ExpressionVariable>> = {}, double minimumValue = 0.0,
                                          double maximumValue = 1.0, double valueOffset = 0.0, double penaltyPower = 1.0);

    protected:
    // Local variables, set when checking voxels
    std::shared_ptr<ExpressionVariable> x_, y_, z_, xFrac_, yFrac_, zFrac_;
    // Expression describing region
    NodeValue expression_;
    // Minimum threshold value for function
    double minimumValue_{0.0};
    // Maximum threshold value for function
    double maximumValue_{1.0};
    // Value offset to use when assessing threshold
    double valueOffset_{0.0};
    // Power law to apply when function value is outside of threshold limits
    double penaltyPower_{1.0};

    private:
    // Set voxel position variables
    void setVoxelPosition(const Box *box, Vec3<double> r) const;
    // Return current value of function, applying any threshold penalty
    double functionValue() const;

    public:
    // Calculate and store energy and force for the specified voxel centre
    void energyAndForce(const Box *box, const Vec3<double> &r, double &energy, Vec3<double> &force) const;
};

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
    // Fractional voxel size
    Vec3<double> voxelSizeFrac_;
    // Vector fields for energy and derived force
    Array3D<double> energyVoxels_;
    Array3D<Vec3<double>> forceVoxels_;

    private:
    // Generate voxel combinable
    static dissolve::CombinableFunctor<std::shared_ptr<RegionalPotentialVoxelKernel>>
    createCombinableVoxelKernel(std::function<std::shared_ptr<RegionalPotentialVoxelKernel>(void)> kernelGenerator)
    {
        return kernelGenerator;
    }
    // Return voxel coordinates of supplied atom
    std::tuple<int, int, int> voxelIndices(const Atom i, const Box *box) const;

    public:
    // Set up potential for supplied box
    bool setUp(const Box *box, double voxelSize,
               const std::function<std::shared_ptr<RegionalPotentialVoxelKernel>(void)> &kernelGenerator);
    // Return functional form of the potential, as a string
    const std::string formString() const override;
    // Return parameters of the potential, as a string
    const std::string formParametersString() const override;

    /*
     * Potential Calculation
     */
    public:
    // Calculate energy on specified atom
    double energy(const Atom i, const Box *box) const override;
    // Calculate force on specified atom, summing in to supplied vector
    void force(const Atom i, const Box *box, Vec3<double> &f) const override;
};
