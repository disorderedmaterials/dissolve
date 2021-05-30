// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "templates/algorithms.h"

/*
 * Private Functions
 */

// Ensure arrays are the correct size for the current target Species
void CalculateAvgMolModule::updateArrays(Dissolve &dissolve)
{
    auto requiredSize = targetSpecies_ ? targetSpecies_->nAtoms() : -1;

    // Retrieve / create the three data arrays, and size accordingly
    auto &x = dissolve.processingModuleData().realise<SampledVector>("X", uniqueName(), GenericItem::InRestartFileFlag);
    auto &y = dissolve.processingModuleData().realise<SampledVector>("Y", uniqueName(), GenericItem::InRestartFileFlag);
    auto &z = dissolve.processingModuleData().realise<SampledVector>("Z", uniqueName(), GenericItem::InRestartFileFlag);

    if (requiredSize > 0)
    {
        if (x.values().size() == requiredSize && y.values().size() == requiredSize && z.values().size() == requiredSize)
            Messenger::print("Using existing coordinate arrays for average species.\n");
        else
        {
            Messenger::printVerbose("Initialising arrays for average molecule: size = {}\n", requiredSize);
            x.initialise(requiredSize);
            y.initialise(requiredSize);
            z.initialise(requiredSize);
        }
    }
    else
    {
        x.clear();
        y.clear();
        z.clear();
    }
}

// Update the local species with the coordinates from the supplied arrays
void CalculateAvgMolModule::updateSpecies(const SampledVector &x, const SampledVector &y, const SampledVector &z)
{
    for (auto &&[i, rx, ry, rz] : zip(averageSpecies_.atoms(), x.values(), y.values(), z.values()))
        averageSpecies_.setAtomCoordinates(&i, {rx, ry, rz});
}

/*
 * Public Functions
 */

// Return average Species
Species &CalculateAvgMolModule::averageSpecies() { return averageSpecies_; }
