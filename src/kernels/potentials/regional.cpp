// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
                                                           double minimumValue, double maximumValue)
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
}

// Calculate and store energy and force for the specified voxel centre
void RegionalPotentialVoxelKernel::energyAndForce(const Box *box, const Vec3<double> &r, double &energy,
                                                  Vec3<double> &force) const
{
    // Poke values into our variables
    x_->setValue(r.x);
    y_->setValue(r.y);
    z_->setValue(r.z);
    auto rFrac = box->getFractional(r);
    xFrac_->setValue(rFrac.x);
    yFrac_->setValue(rFrac.y);
    zFrac_->setValue(rFrac.z);

    // Energy
    energy = expression_.asDouble();
    if (energy < minimumValue_ || energy > maximumValue_)
        energy *= 1000.0;
}

/*
 * Regional Potential
 */

RegionalPotential::RegionalPotential() : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Regional) {}

/*
 * Definition
 */

// Return functional form of the potential, as a string
const std::string RegionalPotential::formString() const { return "Custom"; }

// Return parameters of the potential, as a string
const std::string RegionalPotential::formParametersString() const { return "N/A"; }

// Set up potential for supplied box
bool RegionalPotential::setUp(const Box *box,
                              const std::function<std::shared_ptr<RegionalPotentialVoxelKernel>(void)> &kernelGenerator)
{
    // Set fractional voxel sizes
    Vec3<int> nVoxels;
    for (auto n = 0; n < 3; ++n)
        nVoxels.set(n, std::max(int(box->axisLength(n) / voxelSize_), 1));
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

std::tuple<int, int, int> RegionalPotential::voxelIndices(const Atom &i, const Box *box) const
{
    auto r = box->getFractional(i.r());
    return {r.x / voxelSizeFrac_.x, r.y / voxelSizeFrac_.y, r.z / voxelSizeFrac_.z};
}

// Calculate energy on specified atom
double RegionalPotential::energy(const Atom &i, const Box *box) const { return energyVoxels_[voxelIndices(i, box)]; }

// Calculate force on specified atom, summing in to supplied vector
void RegionalPotential::force(const Atom &i, const Box *box, Vec3<double> &f) const
{
    throw(std::runtime_error(fmt::format("Not implemented yet.\n")));
}
