// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/braggreflection.h"
#include "classes/kvector.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/species.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include "math/pairbroadeningfunction.h"

Dissolve::Dissolve(CoreData &coreData) : coreData_(coreData)
{
    // Set Module instances list in our core data
    coreData_.setModuleInstances(&moduleInstances_);

    // Set core simulation variables
    seed_ = -1;
    restartFileFrequency_ = 10;

    // Clear everything
    clear();
}

Dissolve::~Dissolve()
{
    // Clear main data
    clear();

    // Delete all master Modules
    masterModules_.clear();
}

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
    pairPotentialRangeSquared_ = pairPotentialRange_ * pairPotentialRange_;
    pairPotentialsIncludeCoulomb_ = true;
    pairPotentials_.clear();
    potentialMap_.clear();
    pairPotentialAtomTypeVersion_ = -1;

    // Modules
    Messenger::printVerbose("Clearing Modules...\n");
    moduleInstances_.clear();

    // Simulation
    Messenger::printVerbose("Clearing Simulation...\n");
    processingLayers_.clear();
    processingModuleData_.clearAll();
    iteration_ = 0;
    nIterationsPerformed_ = 0;

    // I/O
    setInputFilename("");
    restartFilename_.clear();
    saveRestartTimes_.clear();
}
