// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/pairPotential.h"
#include "classes/potentialMap.h"
#include "items/list.h"
#include "math/sampledDouble.h"

// Forward Declarations
class ConfigurationAtom;
class Box;
class Cell;
class DissolveGraph;
class Isotopologue;
class Molecule;

// Dissolve Main Class
class Dissolve : public Serialisable
{
    public:
    Dissolve(CoreData &coreData);
    ~Dissolve();

    /*
     * Core
     */
    private:
    // Reference to CoreData
    CoreData &coreData_;

    public:
    // Return reference to CoreData
    CoreData &coreData();
    const CoreData &coreData() const;
    // Clear all data
    void clear();
    // TOML Toggle
    static constexpr bool toml_testing_flag = false;

    /*
     * Pair Potentials
     */
    private:
    // Whether pair potentials are updated automatically through combination rules
    bool useCombinationRules_;
    // Simulation PairPotentials
    std::vector<PairPotential::Definition> pairPotentials_;
    // Map for PairPotentials
    PotentialMap potentialMap_;

    public:
    // Set whether pair potentials are updated automatically through combination rules
    void setUseCombinationRules(bool b);
    // Return whether pair potentials are updated automatically through combination rules
    bool useCombinationRules() const;
    // Return index of specified PairPotential
    int indexOf(PairPotential *pp);
    // Return number of defined PairPotentials
    int nPairPotentials() const;
    // Add new pair potential to list
    PairPotential *addPairPotential(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2);
    // Return PairPotentials list
    const std::vector<PairPotential::Definition> &pairPotentials() const;
    std::vector<PairPotential::Definition> &pairPotentials();
    // Return nth PairPotential in list
    PairPotential *pairPotential(int n);
    // Return specified PairPotential (if defined)
    PairPotential *pairPotential(const AtomType *at1, const AtomType *at2) const;
    PairPotential *pairPotential(std::string_view at1Name, std::string_view at2Name) const;
    // Return map for PairPotentials
    const PotentialMap &potentialMap() const;
    PotentialMap &potentialMap();
    // Update all pair potentials
    bool updatePairPotentials(std::optional<bool> useCombinationRulesHint = {});
    // Clear additional potentials
    void clearAdditionalPotentials();

    /*
     * Processing Module Data
     */
    private:
    // Data associated with processing Modules
    GenericList processingModuleData_;

    public:
    // Return data associated with main processing Modules
    GenericList &processingModuleData();

    /*
     * Graph node
     */
    public:
    // Get graph
    DissolveGraph *graph();
    // Set the Dissolve graph node
    void setGraph();

    private:
    // Dissolve graph node
    std::unique_ptr<DissolveGraph> graphNode_;

    /*
     * Simulation
     */
    private:
    // Frequency at which to write restart file
    int restartFileFrequency_;
    // Current simulation step
    int iteration_;
    // Number of iterations performed
    int nIterationsPerformed_;
    // Main loop iterations timer
    Timer iterationTimer_;
    // Accumulated timing information for main loop iterations
    SampledDouble iterationTime_;

    public:
    // Set frequency with which to write various iteration data
    void setRestartFileFrequency(int n);
    // Return frequency with which to write restart file
    int restartFileFrequency() const;
    // Prepare for main simulation
    bool prepare();

    /*
     * Serialisation
     */
    private:
    // Filename of current input file
    std::string inputFilename_;
    // Filename of current restart file
    std::string restartFilename_;
    // Accumulated timing information for saving restart file
    SampledDouble saveRestartTimes_;

    public:
    // Load input file
    bool loadInput(std::string_view filename);
    // Read pair potentials from a serialisable value
    void deserialisePairPotentials(const SerialisedValue &node);
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    // Save TOML file
    bool saveToml(std::string_view filename) const;
    // Express pair potentials as a serialisable value
    SerialisedValue serialisePairPotentials() const;
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Return whether an input filename has been set
    bool hasInputFilename() const;
    // Set current input filename
    void setInputFilename(std::string_view filename);
    // Return current input filename
    std::string_view inputFilename() const;
    // Return restart filename
    std::string_view restartFilename() const;
    // Return whether a restart filename has been set
    bool hasRestartFilename() const;
};
