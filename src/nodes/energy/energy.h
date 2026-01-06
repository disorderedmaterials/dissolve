// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels//energy.h"
#include "nodes/node.h"

// Forward Declarations
class PotentialMap;

// Energy Module
class EnergyNode : public Node
{
    public:
    EnergyNode(Graph *parentGraph);
    ~EnergyNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Save calculated energies to disk, one file per targeted configuration
    bool save_{false};
    // Threshold value at which energy is deemed stable over the defined windowing period
    double stabilityThreshold_{0.001};
    // Number of points over which to assess the stability of the energy (per Configuration)
    int stabilityWindow_{10};

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
    PairPotentialEnergyValue pairPotentialEnergy(const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total pair potential energy of Species
    double pairPotentialEnergy(const Species *sp, const PotentialMap &potentialMap);
    // Return total intermolecular energy
    double interMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration
    double intraMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total intramolecular energy of Configuration, storing components in provided variables
    double intraMolecularEnergy(const Configuration *cfg, const PotentialMap &potentialMap, double &bondEnergy,
                                double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total intramolecular energy of Species
    double intraMolecularEnergy(const Species *sp);
    // Return total energy (interatomic and intramolecular)
    double totalEnergy(const Configuration *cfg, const PotentialMap &potentialMap);
    // Return total energy (interatomic and intramolecular) of Configuration, storing components in provided variables
    double totalEnergy(const Configuration *cfg, const PotentialMap &potentialMap, PairPotentialEnergyValue &ppEnergy,
                       double &bondEnergy, double &angleEnergy, double &torsionEnergy, double &improperEnergy);
    // Return total energy (interatomic and intramolecular) of Species
    double totalEnergy(const Species *sp, const PotentialMap &potentialMap);

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
