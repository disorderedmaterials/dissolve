// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "base/timer.h"
#include "classes/pairPotential.h"
#include "classes/potentialMap.h"
#include "math/sampledDouble.h"

// Forward Declarations
class ConfigurationAtom;
class Box;
class DissolveGraph;
class Isotopologue;
class Molecule;

// Dissolve Main Class
class Dissolve
{
    public:
    Dissolve();
    ~Dissolve();

    public:
    // Clear all data
    void clear();
    // TOML Toggle
    static constexpr bool toml_testing_flag = false;

    /*
     * Pair Potentials
     */
    private:
    // Simulation PairPotentials
    std::vector<PairPotential::Definition> pairPotentials_;
    // Map for PairPotentials
    PotentialMap potentialMap_;

    public:
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
    // Current simulation step
    int iteration_;
    // Number of iterations performed
    int nIterationsPerformed_;
    // Main loop iterations timer
    Timer iterationTimer_;
    // Accumulated timing information for main loop iterations
    SampledDouble iterationTime_;

    public:
    // Load TOML input file  TO BE DELETED
    bool loadTOML(std::string_view filename);
    // Save TOML input file  TO BE DELETED
    bool saveTOML(std::string_view filename) const;
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
