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

RegionalPotential::RegionalPotential() : ExternalPotential(ExternalPotentialTypes::ExternalPotentialType::Regional)
{
    //    keywords_.add<NodeValueKeyword>("Expression", "Expression describing potential]", expression_);
    //    keywords_.add<DoubleKeyword>("Minimum", "Minimum value for descriptive function defining region", minimumValue_);
    //    keywords_.add<DoubleKeyword>("Maximum", "Maximum value for descriptive function defining region", maximumValue_);
    // Create our local variables
    x_ = expression_.addLocalVariable("x");
    y_ = expression_.addLocalVariable("y");
    z_ = expression_.addLocalVariable("z");
    xFrac_ = expression_.addLocalVariable("xFrac");
    yFrac_ = expression_.addLocalVariable("yFrac");
    zFrac_ = expression_.addLocalVariable("zFrac");

    expression_.set("x");
}

/*
 * Definition
 */

// Return functional form of the potential, as a string
const std::string RegionalPotential::formString() const { return "Custom"; }

// Return parameters of the potential, as a string
const std::string RegionalPotential::formParametersString() const { return "N/A"; }

// Set up potential for supplied box
bool RegionalPotential::setUp(const Box *box)
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
    auto energyVoxelFunction = [&](auto triplet, auto x, auto y, auto z)
    {
        auto r = box->getReal({(x + 0.5) * voxelSizeFrac_.x, (y + 0.5) * voxelSizeFrac_.y, (z + 0.5) * voxelSizeFrac_.z});
        // Poke values into our variables
        x_->setValue(r.x);
        y_->setValue(r.y);
        z_->setValue(r.z);
        auto rFrac = box->getFractional(r);
        xFrac_->setValue(rFrac.x);
        yFrac_->setValue(rFrac.y);
        zFrac_->setValue(rFrac.z);

        auto e = expression_.asDouble();
        energyVoxels_[triplet] = e > 0.2 ? 1000.0 * e : e;
    };

    // Iterate over energy voxels
    dissolve::for_each_triplet(ParallelPolicies::seq, energyVoxels_.beginIndices(), energyVoxels_.endIndices(),
                               energyVoxelFunction);

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
