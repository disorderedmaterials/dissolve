// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// AtomShake Module
class AtomShakeModule : public Module
{
    public:
    AtomShakeModule();
    ~AtomShakeModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
    // Interatomic cutoff distance to use for energy calculation
    double cutoffDistance_{-1.0};
    // Number of shakes to attempt per atom
    int nShakesPerAtom_{1};
    // Step size in Angstroms to use in Monte Carlo moves
    double stepSize_{0.05};
    // Maximum allowed value for step size, in Angstroms
    double stepSizeMax_{1.0};
    // Minimum allowed value for step size, in Angstroms
    double stepSizeMin_{0.001};
    // Target acceptance rate for Monte Carlo moves
    double targetAcceptanceRate_{0.33};

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
