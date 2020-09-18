/*
    *** Dissolve - Communications
    *** src/main/comms.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "main/dissolve.h"

// Parallel Strategy Keywords
std::string_view ParallelStrategyKeywords[] = {"Sequential", "Even"};

// Convert string to ParallelStrategy
Dissolve::ParallelStrategy Dissolve::parallelStrategy(std::string_view s)
{
    for (int n = 0; n < Dissolve::nParallelStrategies; ++n)
        if (DissolveSys::sameString(ParallelStrategyKeywords[n], s))
            return (Dissolve::ParallelStrategy)n;
    return Dissolve::nParallelStrategies;
}

// Set parallel strategy for Configuration work
void Dissolve::setParallelStrategy(Dissolve::ParallelStrategy ps) { parallelStrategy_ = ps; }

// Return parallel strategy for Configuration work
Dissolve::ParallelStrategy Dissolve::parallelStrategy() const { return parallelStrategy_; }

// Set default process group population (per Configuration)
void Dissolve::setParallelGroupPopulation(int groupPopulation) { parallelGroupPopulation_ = groupPopulation; }

// Return default process group population (per Configuration)
int Dissolve::parallelGroupPopulation() const { return parallelGroupPopulation_; }

// Return a world process pool
ProcessPool &Dissolve::worldPool()
{
    static bool firstRun = true;
    static ProcessPool world;

    // If this is the first time we've been called, construct the pool
    if (firstRun)
    {
        // Assemble list of (world) process ranks for the pool
        Array<int> ranks;
        for (int n = 0; n < ProcessPool::nWorldProcesses(); ++n)
            ranks.add(n);
        world.setUp("World", ranks, ProcessPool::MinimumGroupPopulation);
        firstRun = false;
    }

    return world;
}

// Set up communications
bool Dissolve::setUpMPIPools()
{
    Messenger::print("*** Setting up MPI pools...\n");

    // If there is only one process, make sure we revert to SequentialConfigStrategy
    if ((ProcessPool::nWorldProcesses() == 1) && (parallelStrategy_ != Dissolve::SequentialConfigStrategy))
    {
        Messenger::warn("Parallel strategies make no sense with only one processor, so reverting to sequential "
                        "strategy default.\n");
        parallelStrategy_ = Dissolve::SequentialConfigStrategy;
    }

    // Get relative atom counts between each configuration
    Array<int> configSizes;
    for (auto *cfg = coreData_.configurations().first(); cfg != nullptr; cfg = cfg->next())
        configSizes.add(cfg->nAtoms());
    // 	configSizes /= configSizes.min();

    // Default pool - all world ranks
    Array<int> allProcesses;
    for (int n = 0; n < ProcessPool::nWorldProcesses(); ++n)
        allProcesses.add(n);

    // Set up pool based on selected strategy
    auto cfgIndex = 0;
    for (auto *cfg = coreData_.configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        Messenger::print("Configuration '{}':\n", cfg->name());

        if (parallelStrategy_ == Dissolve::SequentialConfigStrategy)
        {
            // Simple, sequential strategy - all processes assigned to all Configurations
            if (!cfg->setUpProcessPool(allProcesses, parallelGroupPopulation_))
                return false;
        }
        else if (parallelStrategy_ == Dissolve::EvenStrategy)
        {
            // All processes divided equally amongst Configurations - do we have enough?
            if (ProcessPool::nWorldProcesses() < nConfigurations())
            {
                Messenger::error("Number of processes ({}) is less than the number of Configurations ({}) so "
                                 "Even strategy can't be employed.\n",
                                 ProcessPool::nWorldProcesses(), nConfigurations());
                return false;
            }
            else if (ProcessPool::nWorldProcesses() % nConfigurations() != 0)
            {
                Messenger::error("Number of processes ({}) does not divide equally amongst the number of "
                                 "Configurations ({}) so Even strategy can't be employed.\n",
                                 ProcessPool::nWorldProcesses(), coreData_.nConfigurations());
                return false;
            }

            // Create new pool
            auto procsPerConfig = ProcessPool::nWorldProcesses() / nConfigurations();
            Array<int> poolProcesses;
            for (int n = 0; n < procsPerConfig; ++n)
                poolProcesses.add(procsPerConfig * cfgIndex + n);
            if (!cfg->setUpProcessPool(poolProcesses, parallelGroupPopulation_))
                return false;
        }

        // Increase Configuration index
        ++cfgIndex;
    }

    return true;
}
