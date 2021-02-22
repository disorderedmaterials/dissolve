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
    ~MDModule() = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    private:
    // Species types to restrict calculation to (if any)
    RefList<Species> restrictToSpecies_;

    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions
     */
    private:
    // Cap forces in Configuration
    int capForces(Configuration *cfg, double maxForceSq, Array<double> &fx, Array<double> &fy, Array<double> &fz);
    // Determine timestep based on maximal force component
    double determineTimeStep(const Array<double> &fx, const Array<double> &fy, const Array<double> &fz);
};
