// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

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
    // Test parallel energy routines against basic serial versions and supplied reference values
    bool test_{false};
    // Use analytic interatomic energies rather than (production) tabulated potentials for tests
    bool testAnalytic_{false};
    // Reference value for interatomic energy against which to test calculated value
    std::optional<double> testReferenceInter_;
    // Reference value for intramolecular energy against which to test calculated value
    std::optional<double> testReferenceIntra_;
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
    // Return total interatomic energy of Configuration
    static double interAtomicEnergy(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total interatomic energy of Species
    static double interAtomicEnergy(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap);
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
                              double &interEnergy, double &bondEnergy, double &angleEnergy, double &torsionEnergy,
                              double &improperEnergy);
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
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
