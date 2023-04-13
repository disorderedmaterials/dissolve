// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/randombuffer.h"
#include "base/timer.h"
#include "classes/box.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/regionaldistributor.h"
#include "kernels/energy.h"
#include "main/dissolve.h"
#include "modules/atomshake/atomshake.h"

// Run main processing
bool AtomShakeModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Retrieve control parameters from Configuration
    auto rCut = cutoffDistance_.value_or(dissolve.pairPotentialRange());
    const auto termScale = 1.0;
    const auto rRT = 1.0 / (.008314472 * targetConfiguration_->temperature());

    // Print argument/parameter summary
    Messenger::print("AtomShake: Cutoff distance is {}\n", rCut);
    Messenger::print("AtomShake: Performing {} shake(s) per Atom\n", nShakesPerAtom_);
    Messenger::print("AtomShake: Step size for adjustments is {:.5f} Angstroms (allowed range is {} <= delta <= {}).\n",
                     stepSize_, stepSizeMin_, stepSizeMax_);
    Messenger::print("AtomShake: Target acceptance rate is {}.\n", targetAcceptanceRate_);
    Messenger::print("\n");

    ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();
    Timer commsTimer(false);

    // Create a Molecule distributor
    RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells(), procPool, strategy);

    // Create a local ChangeStore and EnergyKernel
    ChangeStore changeStore(procPool, commsTimer);
    EnergyKernel kernel(procPool, targetConfiguration_, dissolve.potentialMap(), rCut);

    // Initialise the random number buffer so it is suitable for our parallel strategy within the main loop
    RandomBuffer randomBuffer(procPool, ProcessPool::subDivisionStrategy(strategy), commsTimer);

    int shake, n;
    auto nAttempts = 0, nAccepted = 0;
    bool accept;
    double currentEnergy, currentIntraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
    Vec3<double> rDelta;

    Timer timer;
    while (distributor.cycle())
    {
        // Get next set of Molecule targets from the distributor
        auto &targetMolecules = distributor.assignedMolecules();

        // Switch parallel strategy if necessary
        if (distributor.currentStrategy() != strategy)
        {
            // Set the new strategy
            strategy = distributor.currentStrategy();

            // Re-initialise the random buffer
            randomBuffer.reset(ProcessPool::subDivisionStrategy(strategy));
        }

        // Loop over target Molecules
        for (auto molId : targetMolecules)
        {
            /*
             * Calculation Begins
             */

            // Get Molecule index and pointer
            std::shared_ptr<Molecule> mol = targetConfiguration_->molecule(molId);

            // Set current Atom targets in ChangeStore (whole Molecule)
            changeStore.add(mol);

            n = 0;
            // Loop over atoms in the Molecule
            for (const auto &i : mol->atoms())
            {
                // Calculate reference energies for the Atom
                currentEnergy = kernel.pairPotentialEnergy(*i);
                currentIntraEnergy = kernel.intramolecularEnergy(*mol, *i) * termScale;

                // Loop over number of shakes per Atom
                for (shake = 0; shake < nShakesPerAtom_; ++shake)
                {
                    // Create a random translation vector
                    rDelta.set(randomBuffer.randomPlusMinusOne() * stepSize_, randomBuffer.randomPlusMinusOne() * stepSize_,
                               randomBuffer.randomPlusMinusOne() * stepSize_);

                    // Translate Atom and update its Cell position
                    i->translateCoordinates(rDelta);
                    targetConfiguration_->updateCellLocation(i);

                    // Calculate new energy
                    newEnergy = kernel.pairPotentialEnergy(*i);
                    newIntraEnergy = kernel.intramolecularEnergy(*mol, *i) * termScale;

                    // Trial the transformed Atom position
                    delta = (newEnergy + newIntraEnergy) - (currentEnergy + currentIntraEnergy);
                    accept = delta < 0 ? true : (randomBuffer.random() < exp(-delta * rRT));

                    if (accept)
                    {
                        // Accept new (current) position of target Atom
                        changeStore.updateAtom(n);
                        currentEnergy = newEnergy;
                    }
                    else
                        changeStore.revert(n);

                    // Increase attempt counters
                    // The strategy in force at any one time may vary, so use the distributor's
                    // helper functions.
                    if (distributor.collectStatistics())
                    {
                        if (accept)
                        {
                            totalDelta += delta;
                            ++nAccepted;
                        }
                        ++nAttempts;
                    }
                    ++n;
                }
            }

            // Store modifications to Atom positions ready for broadcast later
            changeStore.storeAndReset();

            /*
             * Calculation End
             */
        }

        // Now all target Molecules have been processes, broadcast the changes made
        changeStore.distributeAndApply(targetConfiguration_);
        changeStore.reset();
    }

    timer.stop();

    // Collect statistics across all processes
    if (!procPool.allSum(&nAccepted, 1, strategy, commsTimer))
        return false;
    if (!procPool.allSum(&nAttempts, 1, strategy, commsTimer))
        return false;
    if (!procPool.allSum(&totalDelta, 1, strategy, commsTimer))
        return false;

    Messenger::print("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

    // Calculate and print acceptance rate
    double rate = double(nAccepted) / nAttempts;
    Messenger::print("Total number of attempted moves was {} ({} work, {} comms)\n", nAttempts, timer.totalTimeString(),
                     commsTimer.totalTimeString());

    Messenger::print("Overall acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rate, nAccepted, nAttempts);

    // Update and set translation step size
    stepSize_ *= (nAccepted == 0) ? 0.8 : rate / targetAcceptanceRate_;
    if (stepSize_ < stepSizeMin_)
        stepSize_ = stepSizeMin_;
    else if (stepSize_ > stepSizeMax_)
        stepSize_ = stepSizeMax_;
    keywords_.set("StepSize", stepSize_);

    Messenger::print("Updated step size is {} Angstroms.\n", stepSize_);

    // Increase contents version in Configuration
    if (nAccepted > 0)
        targetConfiguration_->incrementContentsVersion();

    return true;
}
