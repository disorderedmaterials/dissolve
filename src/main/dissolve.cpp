// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "main/dissolve.h"
#include "classes/kVector.h"
#include "nodes/dissolve.h"

Dissolve::Dissolve()
{
    // Set core simulation variables
    restartFileFrequency_ = 10;

    graphNode_ = std::make_unique<DissolveGraph>();

    // Clear everything
    clear();
}

Dissolve::~Dissolve() { clear(); }

/*
 * Core
 */

DissolveGraph *Dissolve::graph() { return graphNode_.get(); }

// Clear all data
void Dissolve::clear()
{
    // PairPotentials
    Messenger::printVerbose("Clearing Pair Potentials...\n");
    useCombinationRules_ = true;
    PairPotential::setChargeSource(PairPotential::ChargeSource::Automatic);
    pairPotentials_.clear();
    potentialMap_.clear();

    // Simulation
    Messenger::printVerbose("Clearing Simulation...\n");
    iteration_ = 0;
    nIterationsPerformed_ = 0;

    // Graph
    graphNode_ = std::make_unique<DissolveGraph>();

    // I/O
    setInputFilename("");
    restartFilename_.clear();
    saveRestartTimes_.clear();
}
