// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "module/module.h"

// Forward Declarations
/* none */

// Calculate Average Molecule Module
class CalculateAvgMolModule : public Module
{
    public:
    CalculateAvgMolModule();
    ~CalculateAvgMolModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions / Data
     */
    private:
    // Species targeted by module (derived from selected site)
    Species *targetSpecies_;
    // Local Species representing average of targeted Species
    Species averageSpecies_;

    private:
    // Ensure arrays are the correct size for the current target Species
    void updateArrays(Dissolve &dissolve);
    // Update the local species with the coordinates from the supplied arrays
    void updateSpecies(const Array<SampledDouble> &x, const Array<SampledDouble> &y, const Array<SampledDouble> &z);

    public:
    // Return average Species
    Species &averageSpecies();

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
