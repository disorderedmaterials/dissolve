// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "classes/atomType.h"
#include "classes/kVector.h"
#include "classes/neutronWeights.h"
#include "classes/species.h"

Dissolve::Dissolve(CoreData &coreData) : coreData_(coreData)
{
    // Set core simulation variables
    restartFileFrequency_ = 10;

    setUpWorldPool();

    // Clear everything
    clear();
}

Dissolve::~Dissolve() { clear(); }

/*
 * Core
 */

// Return reference to CoreData
CoreData &Dissolve::coreData() { return coreData_; }

const CoreData &Dissolve::coreData() const { return coreData_; }

// Clear all data
void Dissolve::clear()
{
    // Core
    Messenger::printVerbose("Clearing Core Data...\n");
    coreData_.clear();

    // PairPotentials
    Messenger::printVerbose("Clearing Pair Potentials...\n");
    pairPotentialDelta_ = 0.005;
    pairPotentialRange_ = 15.0;
    automaticChargeSource_ = true;
    forceChargeSource_ = false;
    atomTypeChargeSource_ = true;
    pairPotentials_.clear();
    pairPotentialOverrides_.clear();
    potentialMap_.clear();

    // Simulation
    Messenger::printVerbose("Clearing Simulation...\n");
    processingModuleData_.clearAll();
    iteration_ = 0;
    nIterationsPerformed_ = 0;

    // I/O
    setInputFilename("");
    restartFilename_.clear();
    saveRestartTimes_.clear();
}

/*
 * Processing Module Data
 */

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return processingModuleData_; }
