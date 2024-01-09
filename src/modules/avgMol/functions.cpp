// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/avgMol/avgMol.h"
#include "templates/algorithms.h"

/*
 * Private Functions
 */

// Ensure arrays are the correct size for the current target Species
void AvgMolModule::updateArrays(GenericList &moduleData)
{
    auto requiredSize = targetSpecies_ ? targetSpecies_->nAtoms() : -1;

    // Retrieve / create the three data arrays, and size accordingly
    auto &x = moduleData.realise<SampledVector>("X", name(), GenericItem::InRestartFileFlag);
    auto &y = moduleData.realise<SampledVector>("Y", name(), GenericItem::InRestartFileFlag);
    auto &z = moduleData.realise<SampledVector>("Z", name(), GenericItem::InRestartFileFlag);

    if (requiredSize > 0)
    {
        if (x.values().size() == requiredSize && y.values().size() == requiredSize && z.values().size() == requiredSize)
            Messenger::print("Using existing coordinate arrays for average species.\n");
        else
        {
            Messenger::print("Initialising arrays for average molecule: size = {}\n", requiredSize);
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
void AvgMolModule::updateSpecies(const SampledVector &x, const SampledVector &y, const SampledVector &z)
{
    for (auto &&[i, rx, ry, rz] : zip(averageSpecies_.atoms(), x.values(), y.values(), z.values()))
        averageSpecies_.setAtomCoordinates(&i, {rx, ry, rz});
}

/*
 * Public Functions
 */

// Update average Species with coordinates from processing data
void AvgMolModule::updateSpecies(const GenericList &moduleData)
{
    // Check for presence of data arrays
    if (!moduleData.contains("X", name()) || !moduleData.contains("Y", name()) || !moduleData.contains("Z", name()))
        return;

    // Retrieve data arrays
    auto &x = moduleData.value<SampledVector>("X", name());
    auto &y = moduleData.value<SampledVector>("Y", name());
    auto &z = moduleData.value<SampledVector>("Z", name());

    // Update our Species
    updateSpecies(x, y, z);
}

// Return average Species
Species &AvgMolModule::averageSpecies() { return averageSpecies_; }
