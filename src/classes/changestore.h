// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/changedata.h"
#include <memory>

// Forward Declarations
class Atom;
class Cell;
class Molecule;
class Configuration;
class ProcessPool;

// ChangeStore
class ChangeStore
{
    public:
    ChangeStore(ProcessPool &procPool);
    ~ChangeStore() = default;

    /*
     * Watch Targets
     */
    private:
    // List of target atoms (and modification data)
    std::vector<ChangeData> targetAtoms_;

    public:
    // Add atom to watch
    void add(std::shared_ptr<Atom> i);
    // Add molecule to watch
    void add(std::shared_ptr<Molecule> mol);
    // Add cell to watch
    void add(Cell *cell);

    /*
     * Change Data
     */
    private:
    // List of local changes
    std::vector<ChangeData> changes_;
    // Coordinate broadcast arrays
    std::vector<double> x_, y_, z_;
    // Index broadcast array
    std::vector<int> indices_;

    public:
    // Reset ChangeStore, forgetting all changes
    void reset();
    // Update all Atom positions
    void updateAll();
    // Update single atom position
    void updateAtom(int id);
    // Revert all atoms to stored positions
    void revertAll();
    // Revert specified index to stored position
    void revert(int id);
    // Save Atom changes for broadcast, and reset arrays for new data
    void storeAndReset();

    /*
     * Parallel Comms
     */
    private:
    // ProcessPool over which this ChangeStore should broadcast
    ProcessPool &processPool_;

    public:
    // Distribute and apply change data to all processes
    bool distributeAndApply(Configuration *cfg);
};
