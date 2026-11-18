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
    // Return current input file  TO BE DELETED
    std::filesystem::path inputFile() const;
    // Load TOML input file  TO BE DELETED
    bool loadTOML(std::string_view filename);
    // Save TOML input file  TO BE DELETED
    bool saveTOML(std::string_view filename) const;
    // Prepare for main simulation  TO BE DELETED
    bool prepare();
};
