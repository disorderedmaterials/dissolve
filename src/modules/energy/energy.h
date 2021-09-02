// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Forward Declarations
class PotentialMap;

// Energy Module
class EnergyModule : public Module
{
    /*
     * Calculates the total energy of the system by one of several methods
     */

    public:
    EnergyModule();
    ~EnergyModule() override = default;

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
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;

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
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
