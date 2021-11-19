// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// IntraShake Module
class IntraShakeModule : public Module
{
    public:
    IntraShakeModule();
    ~IntraShakeModule() override = default;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
    // Whether Bonds in the molecule should be shaken
    bool adjustBonds_{true};
    // Whether Angles in the molecule should be shaken
    bool adjustAngles_{true};
    // Whether Torsions in the molecule should be shaken
    bool adjustTorsions_{true};
    // Step size for angle adjustments (degrees)
    double angleStepSize_{5.0};
    // Minimum step size for angle adjustments (degrees)
    double angleStepSizeMin_{0.01};
    // Maximum step size for angle adjustments (degrees)
    double angleStepSizeMax_{20.0};
    // Step size for bond adjustments (Angstroms)
    double bondStepSize_{0.01};
    // Minimum step size for bond adjustments (Angstroms)
    double bondStepSizeMin_{0.001};
    // Maximum step size for bond adjustments (Angstroms)
    double bondStepSizeMax_{0.2};
    // Interatomic cutoff distance to employ
    double cutoffDistance_{-1.0};
    // Number of shakes per term
    int nShakesPerTerm_{1};
    // Target acceptance rate for Monte Carlo moves
    double targetAcceptanceRate_{0.33};
    // Whether only the energy of the intramolecular term is calculated and assessed
    bool termEnergyOnly_{false};
    // Step size for torsion adjustments (degrees)
    double torsionStepSize_{10.0};
    // Minimum step size for torsion adjustments (degrees)
    double torsionStepSizeMin_{0.5};
    // Maximum step size for torsion adjustments (degrees)
    double torsionStepSizeMax_{45.0};

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
