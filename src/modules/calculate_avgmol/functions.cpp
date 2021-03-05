// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"

/*
 * Private Functions
 */

// Ensure arrays are the correct size for the current target Species
void CalculateAvgMolModule::updateArrays(Dissolve &dissolve)
{
    auto requiredSize = targetSpecies_ ? targetSpecies_->nAtoms() : -1;

    // Retrieve / create the three data arrays, and size accordingly
    auto &x = dissolve.processingModuleData().realise<Array<SampledDouble>>("X", uniqueName(), GenericItem::InRestartFileFlag);
    auto &y = dissolve.processingModuleData().realise<Array<SampledDouble>>("Y", uniqueName(), GenericItem::InRestartFileFlag);
    auto &z = dissolve.processingModuleData().realise<Array<SampledDouble>>("Z", uniqueName(), GenericItem::InRestartFileFlag);

    if (requiredSize > 0)
    {
        if (x.nItems() == requiredSize && y.nItems() == requiredSize && z.nItems() == requiredSize)
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
void CalculateAvgMolModule::updateSpecies(const Array<SampledDouble> &x, const Array<SampledDouble> &y,
                                          const Array<SampledDouble> &z)
{
    // Loop over atoms in our species
    for (auto n = 0; n < averageSpecies_.nAtoms(); ++n)
        averageSpecies_.setAtomCoordinates(n, x.at(n).value(), y.at(n).value(), z.at(n).value());
}

/*
 * Public Functions
 */

// Return average Species
Species &CalculateAvgMolModule::averageSpecies() { return averageSpecies_; }
