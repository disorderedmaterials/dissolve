// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "main/dissolve.h"
#include "classes/kVector.h"
#include "nodes/dissolve.h"

Dissolve::Dissolve(CoreData &coreData) : coreData_(coreData)
{
    // Set core simulation variables
    restartFileFrequency_ = 10;

    graphNode_ = std::make_unique<DissolveGraph>(*this);

    // Clear everything
    clear();
}

Dissolve::~Dissolve() { clear(); }

/*
 * Core
 */

DissolveGraph *Dissolve::graph() { return graphNode_.get(); }

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
    useCombinationRules_ = true;
    PairPotential::setChargeSource(PairPotential::ChargeSource::Automatic);
    pairPotentials_.clear();
    potentialMap_.clear();

    // Simulation
    Messenger::printVerbose("Clearing Simulation...\n");
    processingModuleData_.clearAll();
    iteration_ = 0;
    nIterationsPerformed_ = 0;

    // Graph
    graphNode_ = std::make_unique<DissolveGraph>(*this);

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
