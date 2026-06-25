// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include <cstdio>

// Set frequency with which to write various iteration data
void Dissolve::setRestartFileFrequency(int n) { restartFileFrequency_ = n; }

// Return frequency with which to write restart file
int Dissolve::restartFileFrequency() const { return restartFileFrequency_; }

// Prepare for main simulation
bool Dissolve::prepare()
{
    Messenger::banner("Preparing Simulation");

    // Check Species
    // TODO DISSOLVE2
    // for (const auto &sp : coreData_.species())
    // {
    //     if (!sp->checkSetUp())
    //         return false;
    // }

    // Store / update last-used pair potential cutoff
    // If lastPairPotentialCutoff is nullopt, store the current value and move on leaving the cutoff to use as nullopt.
    static std::optional<double> lastPairPotentialRange;
    std::optional<double> newPairPotentialRange;
    if (!lastPairPotentialRange)
        lastPairPotentialRange = PairPotential::range();
    else if (lastPairPotentialRange != PairPotential::range())
    {
        lastPairPotentialRange = PairPotential::range();
        newPairPotentialRange = PairPotential::range();
    }

    // Make sure pair potentials are up-to-date
    if (!updatePairPotentials())
        return false;

    // Check Configurations
    // TODO DISSOLVE2
    // std::set<const Species *> globalUsedSpecies;
    // for (auto &cfg : coreData_.configurations())
    // {
    //     if (newPairPotentialRange)
    //         cfg->updateCells();
    //
    //     // Check Box extent against pair potential range
    //     auto maxPPRange = cfg->box().inscribedSphereRadius();
    //     if (PairPotential::range() > maxPPRange)
    //         return Messenger::error("PairPotential range ({}) is longer than the shortest non-minimum image distance ({}).\n",
    //                                 PairPotential::range(), maxPPRange);
    //
    //     // Update species usage for the next check
    //     for (auto &[sp, pop] : cfg->speciesPopulations())
    //         globalUsedSpecies.emplace(sp);
    // }
    //
    // // If we have no configurations, check all species regardless
    // if (coreData_.nConfigurations() == 0)
    //     for (const auto &sp : coreData_.species())
    //         globalUsedSpecies.emplace(sp.get());
}
