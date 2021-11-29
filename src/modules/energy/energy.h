// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    std::vector<Configuration *> targetConfigurations_;
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
    double testReferenceInter_{0.0};
    // Reference value for intramolecular energy against which to test calculated value
    double testReferenceIntra_{0.0};
    // Threshold of energy at which test comparison will fail
    double testThreshold_{0.1};

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
    static double interAtomicEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total interatomic energy of Species
    static double interAtomicEnergy(ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap);
    // Return total intermolecular energy
    static double interMolecularEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration
    static double intraMolecularEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration, storing components in provided variables
    static double intraMolecularEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                                       double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total intramolecular energy of Species
    static double intraMolecularEnergy(const Species *sp);
    // Return total energy (interatomic and intramolecular)
    static double totalEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
    static double totalEnergy(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                              double &interEnergy, double &bondEnergy, double &angleEnergy, double &torsionEnergy,
                              double &improperEnergy);
    // Return total energy (interatomic and intramolecular) of Species
    static double totalEnergy(ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap);
    // Check energy stability of specified Configuration
    static EnergyStability checkStability(GenericList &processingData, const Configuration *cfg);
    // Check energy stability of specified Configurations, returning the number that are unstable
    static int nUnstable(GenericList &processingData, const std::vector<Configuration *> &configurations);

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
