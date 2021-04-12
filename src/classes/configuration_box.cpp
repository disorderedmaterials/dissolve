// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "modules/energy/energy.h"

// Create Box definition with  specifeid lengths and angles
bool Configuration::createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic)
{
    // Remove old box if present
    if (box_ != nullptr)
    {
        Messenger::printVerbose("Removing existing Box definition for Configuration '{}'...\n", niceName());
        delete box_;
        box_ = nullptr;
    }

    if (nonPeriodic)
        box_ = new NonPeriodicBox(1.0);
    else
        box_ = Box::generate(lengths, angles);

    return true;
}

// Return Box
const Box *Configuration::box() const { return box_; }

// Scale Box lengths (and associated Cells) by specified factor
void Configuration::scaleBox(double factor)
{
    // Scale Box
    box_->scale(factor);

    // Apply factor to Cells
    cells_.scale(factor);
}

// Set requested size factor for Box
void Configuration::setRequestedSizeFactor(double factor) { requestedSizeFactor_ = factor; }

// Return requested size factor for Box
double Configuration::requestedSizeFactor() const { return requestedSizeFactor_; }

// Return last size factor applied to Box / Cells
double Configuration::appliedSizeFactor() const { return appliedSizeFactor_; }

// Set requested side length for individual Cell
void Configuration::setRequestedCellDivisionLength(double a) { requestedCellDivisionLength_ = a; }

// Return requested side length for individual Cell
double Configuration::requestedCellDivisionLength() const { return requestedCellDivisionLength_; }

// Return cell array
CellArray &Configuration::cells() { return cells_; }

const CellArray &Configuration::cells() const { return cells_; }

// Scale Box, Cells, and Molecule geometric centres according to current size factor
void Configuration::applySizeFactor(const PotentialMap &potentialMap)
{
    const auto reductionFactor = 0.95;

    while (true)
    {
        // Calculate ratio between current and applied size factors for use later on
        const auto sizeFactorRatio = requestedSizeFactor_ / appliedSizeFactor_;

        // Check current vs applied size factors (via the ratio) - if unequal, perform scaling and set the new applied
        // size factor
        if (fabs(sizeFactorRatio - 1.0) > 1.0e-5)
        {
            Messenger::print("Requested SizeFactor for Configuration is {}, current SizeFactor is {}, so scaling "
                             "Box contents.\n",
                             requestedSizeFactor_, appliedSizeFactor_);

            // Scale molecule centres of geometry
            scaleMoleculeCentres(sizeFactorRatio);

            // Now scale the Box and its Cells
            scaleBox(sizeFactorRatio);

            // Re-assign all Atoms to Cells
            updateCellContents();

            // Store new size factors
            appliedSizeFactor_ = requestedSizeFactor_;

            // Can now break out of the loop
            break;
        }

        /*
         * Now check the current sizeFactor or energy
         *  -- If the current sizeFactor is 1.0, break
         *  -- Otherwise, check energy - if it is negative, reduce requested size factor
         *  -- If energy is positive, don't change anything
         */
        if (fabs(requestedSizeFactor_ - 1.0) < 1.0e-5)
            break;
        else if (EnergyModule::interMolecularEnergy(processPool_, this, potentialMap) <= 0.0)
        {
            requestedSizeFactor_ *= reductionFactor;
            if (requestedSizeFactor_ < 1.0)
                requestedSizeFactor_ = 1.0;
            Messenger::print("Intermolecular energy is zero or negative, so reducing SizeFactor to {}\n", requestedSizeFactor_);
        }
        else
        {
            Messenger::print("Intermolecular energy is positive, so SizeFactor remains at {}\n", requestedSizeFactor_);
            break;
        }
    }

    ++contentsVersion_;
}
