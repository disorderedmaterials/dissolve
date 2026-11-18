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

    public:
    // Load TOML input file  TO BE DELETED
    bool loadTOML(std::string_view filename);
    // Save TOML input file  TO BE DELETED
    bool saveTOML(std::string_view filename) const;
    // Prepare for main simulation  TO BE DELETED
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
