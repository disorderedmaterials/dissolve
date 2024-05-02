// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "io/export/species.h"
#include "math/sampledVector.h"
#include "module/module.h"

class GenericList;

// Calculate Average Molecule Module
class AvgMolModule : public Module
{
    public:
    AvgMolModule();
    ~AvgMolModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target site
    const SpeciesSite *targetSite_{nullptr};
    // Whether to save average coordinates to disk
    SpeciesExportFileFormat exportFileAndFormat_;
    // Species targeted by module (derived from selected site)
    const Species *targetSpecies_{nullptr};
    // Local Species representing average of targeted Species
    Species averageSpecies_;

    /*
     * Functions
     */
    private:
    // Ensure arrays are the correct size for the current target Species
    void updateArrays(GenericList &moduleData);
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
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
