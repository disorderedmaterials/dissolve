// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/potentials/regional.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "expression/variable.h"
#include "kernels/potentials/types.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"
#include "templates/algorithms.h"

/*
 * Regional Potential Voxel Kernel
 */

RegionalPotentialVoxelKernel::RegionalPotentialVoxelKernel(std::string_view expressionString,
                                                           std::vector<std::shared_ptr<ExpressionVariable>> parameters,
                                                           double minimumValue, double maximumValue, double valueOffset,
                                                           double penaltyPower)
{
    // Create our local variables
    x_ = expression_.addLocalVariable("x");
    y_ = expression_.addLocalVariable("y");
    z_ = expression_.addLocalVariable("z");
    xFrac_ = expression_.addLocalVariable("xFrac");
    yFrac_ = expression_.addLocalVariable("yFrac");
    zFrac_ = expression_.addLocalVariable("zFrac");

    // Set the expression
    expression_.set(expressionString, std::move(parameters));

    // Set limits
    minimumValue_ = minimumValue;
    maximumValue_ = maximumValue;
    valueOffset_ = valueOffset;
    penaltyPower_ = penaltyPower;
}

// Set voxel position variables
void RegionalPotentialVoxelKernel::setVoxelPosition(const Box *box, Vec3<double> r) const
{
    x_->setValue(r.x);
    y_->setValue(r.y);
    z_->setValue(r.z);
    box->toFractional(r);
    xFrac_->setValue(r.x);
    yFrac_->setValue(r.y);
    zFrac_->setValue(r.z);
}

// Return current value of function, applying any threshold penalty
double RegionalPotentialVoxelKernel::functionValue() const
{
    auto x = expression_.asDouble() + valueOffset_;
    if (x < minimumValue_ || x > maximumValue_)
        x = pow(x, penaltyPower_);
    return x - valueOffset_;
}

// Calculate and store energy and force for the specified voxel centre
void RegionalPotentialVoxelKernel::energyAndForce(const Box *box, const Vec3<double> &r, double &energy,
                                                  Vec3<double> &force) const
{
    // Energy at the centre of the voxel
    setVoxelPosition(box, r);
    energy = functionValue();

    // Force at the centre of the voxel
    const auto rDelta = 0.01;
    for (auto a = 0; a < 3; ++a)
    {
        // Evaluate r(i) + delta
        setVoxelPosition(box, r.adjusted(a, rDelta));
        force[a] = functionValue();

        // Evaluate r(i) - delta
        setVoxelPosition(box, r.adjusted(a, -rDelta));
        force[a] -= functionValue();

        // Finalise force
        force[a] /= -rDelta * 2;
    }
}

/*
 * Regional Potential
 */

RegionalPotential::RegionalPotential() : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Regional) {}

/*
 * Definition
 */

// Return functional form of the potential, as a string
const std::string RegionalPotential::formString() const { return "Expression"; }

// Return parameters of the potential, as a string
const std::string RegionalPotential::formParametersString() const { return "N/A"; }

// Set up potential for supplied box
bool RegionalPotential::setUp(const Box *box, double voxelSize,
                              const std::function<std::shared_ptr<RegionalPotentialVoxelKernel>(void)> &kernelGenerator)
{
    // Set fractional voxel sizes
    Vec3<int> nVoxels;
    for (auto n = 0; n < 3; ++n)
        nVoxels.set(n, std::max(int(box->axisLength(n) / voxelSize), 1));
    voxelSizeFrac_.set(1.0 / nVoxels.x, 1.0 / nVoxels.y, 1.0 / nVoxels.z);

    // Initialise 3D maps
    energyVoxels_.initialise(nVoxels.x, nVoxels.y, nVoxels.z);
    forceVoxels_.initialise(nVoxels.x, nVoxels.y, nVoxels.z);

    // Create a voxel combinable and check function
    auto voxelCombinable = createCombinableVoxelKernel(kernelGenerator);
    auto voxelFunction = [&](auto triplet, auto x, auto y, auto z)
    {
        auto r = box->getReal({(x + 0.5) * voxelSizeFrac_.x, (y + 0.5) * voxelSizeFrac_.y, (z + 0.5) * voxelSizeFrac_.z});
        voxelCombinable.local()->energyAndForce(box, r, energyVoxels_[triplet], forceVoxels_[triplet]);
    };

    // Iterate over voxel indices
    dissolve::for_each_triplet(ParallelPolicies::par, energyVoxels_.beginIndices(), energyVoxels_.endIndices(), voxelFunction);

    return true;
}

/*
 * Potential Calculation
 */

std::tuple<int, int, int> RegionalPotential::voxelIndices(const Atom i, const Box *box) const
{
    auto r = box->getFractional(i.r());
    return {r.x / voxelSizeFrac_.x, r.y / voxelSizeFrac_.y, r.z / voxelSizeFrac_.z};
}

// Calculate energy on specified atom
double RegionalPotential::energy(const Atom i, const Box *box) const { return energyVoxels_[voxelIndices(i, box)]; }

// Calculate force on specified atom, summing in to supplied vector
void RegionalPotential::force(const Atom i, const Box *box, Vec3<double> &f) const { f = forceVoxels_[voxelIndices(i, box)]; }
