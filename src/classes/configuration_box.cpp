// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "kernels/energy.h"

// Create Box definition with specified lengths and angles
void Configuration::createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    box_ = Box::generate(lengths, angles, nonPeriodic);

    cells_.clear();
}

// Create Box definition from axes matrix
void Configuration::createBox(const Matrix3 axes)
{
    // Calculate cell lengths
    Vector3 lengths(axes.columnMagnitude(0), axes.columnMagnitude(1), axes.columnMagnitude(2));

    // Calculate cell angles
    Vector3 vecx, vecy, vecz;
    vecx = axes.columnAsVec3(0);
    vecy = axes.columnAsVec3(1);
    vecz = axes.columnAsVec3(2);
    vecx.normalise();
    vecy.normalise();
    vecz.normalise();

    Vector3 angles(acos(vecy.dp(vecz)), acos(vecx.dp(vecz)), acos(vecx.dp(vecy)));
    angles.toDegrees();

    box_ = Box::generate(lengths, angles);

    cells_.clear();
}

// Create Box definition with specified lengths and angles, and initialise cell array
void Configuration::createBoxAndCells(const Vector3 lengths, const Vector3 angles, bool nonPeriodic)
{
    createBox(lengths, angles, nonPeriodic);
    cells_.generate(box_.get(), requestedCellDivisionLength_);
}

// Create Box definition from axes matrix, and initialise cell array
void Configuration::createBoxAndCells(const Matrix3 axes)
{
    // Forcibly clear the cell array so we ensure that it is regenerated following the box change
    cells_.clear();

    createBox(axes);
    cells_.generate(box_.get(), requestedCellDivisionLength_);
}

// Update cell array, and reassign atoms to cells
void Configuration::updateCells()
{
    cells_.generate(box_.get(), requestedCellDivisionLength_);
    updateAtomLocations(true);
}

// Return Box
const Box *Configuration::box() const { return box_.get(); }

// Scale Box lengths (and associated Cells) by specified factors
void Configuration::scaleBox(Vector3 scaleFactors)
{
    box_->scale(scaleFactors);
    cells_.scale(scaleFactors);

    ++version_;
}

// Set requested size factor for Box
void Configuration::setRequestedSizeFactor(double factor) { requestedSizeFactor_ = factor; }

// Return requested size factor for Box
std::optional<double> Configuration::requestedSizeFactor() const { return requestedSizeFactor_; }

// Return last size factor applied to Box / Cells
std::optional<double> Configuration::appliedSizeFactor() const { return appliedSizeFactor_; }

// Set requested side length for individual Cell
void Configuration::setRequestedCellDivisionLength(double a) { requestedCellDivisionLength_ = a; }

// Return requested side length for individual Cell
double Configuration::requestedCellDivisionLength() const { return requestedCellDivisionLength_; }

// Return cell array
CellArray &Configuration::cells() { return cells_; }
const CellArray &Configuration::cells() const { return cells_; }

// Scale Box, Cells, and Molecule geometric centres according to current size factor
void Configuration::applySizeFactor(const EnergyKernel *kernel)
{
    const auto reductionFactor = 0.95;

    auto requestedSF = requestedSizeFactor_.value_or(defaultSizeFactor_);
    auto appliedSF = appliedSizeFactor_.value_or(defaultSizeFactor_);

    while (true)
    {
        // Calculate ratio between current and applied size factors for use later on
        const auto sizeFactorRatio = requestedSF / appliedSF;

        // Check current vs applied size factors (via the ratio) - if unequal, perform scaling and set the new applied
        // size factor
        if (fabs(sizeFactorRatio - 1.0) > 1.0e-5)
        {
            Messenger::print("Requested SizeFactor for Configuration is {}, current SizeFactor is {}, so scaling "
                             "Box contents.\n",
                             requestedSF, appliedSF);

            // Scale molecule centres of geometry
            scaleContents({sizeFactorRatio, sizeFactorRatio, sizeFactorRatio});

            // Now scale the Box and its Cells
            scaleBox({sizeFactorRatio, sizeFactorRatio, sizeFactorRatio});

            // Re-assign all Atoms to Cells
            updateAtomLocations();

            // Store new size factors
            appliedSizeFactor_ = requestedSF;

            // Can now break out of the loop
            break;
        }

        /*
         * Now check the current sizeFactor or energy
         *  -- If the current sizeFactor is 1.0, break
         *  -- Otherwise, check energy - if it is negative, reduce requested size factor
         *  -- If energy is positive, don't change anything
         */
        if (fabs(requestedSF - 1.0) < 1.0e-5)
        {
            appliedSizeFactor_ = std::nullopt;
            break;
        }
        else if (kernel
                     ->totalEnergy(
                         {Kernel::ExcludeExtended, Kernel::ExcludeGeometric, Kernel::ExcludeIntraMolecularPairPotential})
                     .total() <= 0.0)
        {
            requestedSF *= reductionFactor;
            if (requestedSF < 1.0)
                requestedSF = 1.0;
            Messenger::print("Intermolecular energy is zero or negative, so reducing SizeFactor to {}\n", requestedSF);
            requestedSizeFactor_ = requestedSF;
        }
        else
        {
            Messenger::print("Intermolecular energy is positive, so SizeFactor remains at {}\n", requestedSF);
            break;
        }
    }
}
