// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "math/sampledvector.h"
#include "module/module.h"

// Calculate Average Molecule Module
class CalculateAvgMolModule : public Module
{
    public:
    CalculateAvgMolModule();
    ~CalculateAvgMolModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target site
    const SpeciesSite *targetSite_;
    // Species targeted by module (derived from selected site)
    const Species *targetSpecies_{nullptr};
    // Local Species representing average of targeted Species
    Species averageSpecies_;

    /*
     * Functions
     */
    private:
    // Ensure arrays are the correct size for the current target Species
    void updateArrays(Dissolve &dissolve);
    // Update the local species with the coordinates from the supplied arrays
    void updateSpecies(const SampledVector &x, const SampledVector &y, const SampledVector &z);

    public:
    // Update average Species with coordinates from processing data
    void updateSpecies(const GenericList &moduleData);
    // Return average Species
    Species &averageSpecies();

    /*
     * Processing
     */
    private:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
