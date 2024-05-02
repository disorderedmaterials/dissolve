// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "kernels//energy.h"
#include "module/module.h"

// Forward Declarations
class PotentialMap;

// Energy Module
class EnergyModule : public Module
{
    public:
    EnergyModule();
    ~EnergyModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Save calculated energies to disk, one file per targetted configuration
    bool save_{false};
    // Threshold value at which energy is deemed stable over the defined windowing period
    double stabilityThreshold_{0.001};
    // Number of points over which to assess the stability of the energy (per Configuration)
    int stabilityWindow_{10};
    // Test production energy against analytic "correct" values
    bool test_{false};
    // Threshold of energy at which test comparison will fail
    double testThreshold_{1.0e-6};

    /*
     * Functions
     */
    public:
    // Energy Stability Enum
    enum EnergyStability
    {
        NotAssessable = -1,
        EnergyStable = 0,
        EnergyUnstable = 1
    };
    // Return total pair potential energy of Configuration
    static PairPotentialEnergyValue pairPotentialEnergy(const ProcessPool &procPool, const Configuration *cfg,
                                                        const PotentialMap &potentialMap);
    // Return total pair potential energy of Species
    static double pairPotentialEnergy(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap);
    // Return total intermolecular energy
    static double interMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration
    static double intraMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration, storing components in provided variables
    static double intraMolecularEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                                       double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total intramolecular energy of Species
    static double intraMolecularEnergy(const Species *sp);
    // Return total energy (interatomic and intramolecular)
    static double totalEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
    static double totalEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                              PairPotentialEnergyValue &ppEnergy, double &bondEnergy, double &angleEnergy,
                              double &torsionEnergy, double &improperEnergy);
    // Return total energy (interatomic and intramolecular) of Species
    static double totalEnergy(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap);
    // Check energy stability of specified Configuration
    static EnergyStability checkStability(GenericList &processingData, const Configuration *cfg);
    // Check energy stability of specified Configurations, returning the number that are unstable
    static int nUnstable(GenericList &processingData, const std::vector<Configuration *> &configurations);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
