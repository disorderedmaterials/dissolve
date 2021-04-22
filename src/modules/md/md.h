// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Forward Declarations
class Species;

// Molecular Dynamics Module
class MDModule : public Module
{
    /*
     * Module for testing various functions
     */

    public:
    MDModule();
    ~MDModule() override = default;

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
    private:
    // Species types to restrict calculation to (if any)
    RefList<Species> restrictToSpecies_;

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions
     */
    private:
    // Cap forces in Configuration
    int capForces(double maxForceSq, std::vector<Vec3<double>> &f);
    // Determine timestep based on maximal force component
    double determineTimeStep(const std::vector<Vec3<double>> &f);
};
