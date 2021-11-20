// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// MolShake Module
class MolShakeModule : public Module
{
    public:
    MolShakeModule();
    ~MolShakeModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
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

    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
