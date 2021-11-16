// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// MolShake Module
class MolShakeModule : public Module
{
    /*
     * Performs a molecular Monte Carlo cycle
     */

    public:
    MolShakeModule();
    ~MolShakeModule() override = default;

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
    // Interatomic cutoff distance to use for energy calculation
    double cutoffDistance_{-1.0};
    // Number of shakes to attempt per molecule
    int nShakesPerMolecule_{1};
    // Species to restrict calculation to
    std::vector<const Species *> restrictToSpecies_;
    //  Step size in degrees to use for the rotational component of the Monte Carlo moves
    double rotationStepSize_{1.0};
    // Minimum step size for rotations (degrees)
    double rotationStepSizeMin_{0.01};
    // Maximum step size for rotations (degrees)
    double rotationStepSizeMax_{90.0};
    // Target acceptance rate for Monte Carlo moves
    double targetAcceptanceRate_{0.33};
    // Step size in Angstroms for the translational component of the Monte Carlo moves
    double translationStepSize_{0.05};
    // Minimum step size for translations (Angstroms)
    double translationStepSizeMin_{0.001};
    // Maximum step size for translations (Angstroms)
    double translationStepSizeMax_{3.0};

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
