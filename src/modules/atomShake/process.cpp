// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/timer.h"
#include "classes/box.h"
#include "classes/changeStore.h"
#include "classes/configuration.h"
#include "classes/regionalDistributor.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include "modules/atomShake/atomShake.h"

// Run main processing
Module::ExecutionResult AtomShakeModule::process(Dissolve &dissolve)
{
    // Retrieve control parameters from Configuration
    auto rCut = cutoffDistance_.value_or(PairPotential::range());
    const auto rRT = 1.0 / (.008314472 * targetConfiguration_->temperature());

    // Print argument/parameter summary
    Messenger::print("AtomShake: Cutoff distance is {}\n", rCut);
    Messenger::print("AtomShake: Performing {} shake(s) per Atom\n", nShakesPerAtom_);
    Messenger::print("AtomShake: Step size for adjustments is {:.5f} Angstroms (allowed range is {} <= delta <= {}).\n",
                     stepSize_, stepSizeMin_, stepSizeMax_);
    Messenger::print("AtomShake: Target acceptance rate is {}.\n", targetAcceptanceRate_);
    Messenger::print("\n");

    // Create a Molecule distributor
    RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells());

    // Create a local ChangeStore and EnergyKernel
    ChangeStore changeStore;
    auto kernel = KernelProducer::energyKernel(targetConfiguration_, dissolve.potentialMap());

    auto nAttempts = 0, nAccepted = 0;
    bool accept;
    double currentEnergy, currentIntraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
    Vector3 rDelta;
    Kernel::EnergyResult er;

    Timer timer;
    while (distributor.cycle())
    {
        // Get next set of Molecule targets from the distributor
        auto &targetMolecules = distributor.assignedMolecules();

        // Loop over target Molecules
        for (auto molId : targetMolecules)
        {
            /*
             * Calculation Begins
             */

            // Get Molecule pointer
            std::shared_ptr<Molecule> mol = targetConfiguration_->molecule(molId);

            // Set current Atom targets in ChangeStore (whole Molecule)
            changeStore.add(mol);
            auto storeIndex = 0;

            // Loop over atoms in the Molecule
            for (const auto &i : mol->atoms())
            {
                // Calculate reference energies for the Atom
                er = kernel->totalEnergy(*i);
                currentEnergy = er.totalUnbound();
                currentIntraEnergy = er.geometry.total();

                // Loop over number of shakes per Atom
                for (auto n = 0; n < nShakesPerAtom_; ++n)
                {
                    // Create a random translation vector
                    rDelta.set(DissolveMath::randomPlusMinusOne() * stepSize_, DissolveMath::randomPlusMinusOne() * stepSize_,
                               DissolveMath::randomPlusMinusOne() * stepSize_);

                    // Translate Atom and update its Cell position
                    i->translateCoordinates(rDelta);
                    targetConfiguration_->updateAtomLocation(i);

                    // Calculate new energy
                    er = kernel->totalEnergy(*i);
                    newEnergy = er.totalUnbound();
                    newIntraEnergy = er.geometry.total();

                    // Trial the transformed Atom position
                    delta = (newEnergy + newIntraEnergy) - (currentEnergy + currentIntraEnergy);
                    accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));

                    if (accept)
                    {
                        // Accept new (current) position of target Atom
                        changeStore.updateAtom(storeIndex);
                        currentEnergy = newEnergy;
                    }
                    else
                        changeStore.revert(storeIndex);

                    // Increase attempt counters
                    if (accept)
                    {
                        totalDelta += delta;
                        ++nAccepted;
                    }
                    ++nAttempts;
                }

                // Increment index of target atom in ChangeStore
                ++storeIndex;
            }

            // Store modifications to Atom positions ready for broadcast later
            changeStore.storeAndReset();

            /*
             * Calculation End
             */
        }

        // Now all target Molecules have been processes, broadcast the changes made
        changeStore.apply(targetConfiguration_);
        changeStore.reset();
    }

    timer.stop();

    Messenger::print("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

    // Calculate and print acceptance rate
    double rate = double(nAccepted) / nAttempts;
    Messenger::print("Total number of attempted moves was {} ({})\n", nAttempts, timer.totalTimeString());

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
        targetConfiguration_->notifyAtomicPositionsChanged();

    return ExecutionResult::Success;
}
