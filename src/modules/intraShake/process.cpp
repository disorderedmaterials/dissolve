// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/randomBuffer.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/changeStore.h"
#include "classes/regionalDistributor.h"
#include "classes/species.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/intraShake/intraShake.h"

// Run main processing
Module::ExecutionResult IntraShakeModule::process(ModuleContext &moduleContext)
{
    // Retrieve control parameters
    auto rCut = cutoffDistance_.value_or(moduleContext.dissolve().pairPotentialRange());
    const auto rRT = 1.0 / (.008314472 * targetConfiguration_->temperature());

    // Print argument/parameter summary
    Messenger::print("IntraShake: Cutoff distance is {}\n", rCut);
    Messenger::print("IntraShake: Performing {} shake(s) per term\n", nShakesPerTerm_);
    if (adjustBonds_)
        Messenger::print("IntraShake: Distance step size for bond adjustments is {:.5f} Angstroms (allowed range "
                         "is {} <= delta <= {}).\n",
                         bondStepSize_, bondStepSizeMin_, bondStepSizeMax_);
    if (adjustAngles_)
        Messenger::print("IntraShake: Angle step size for angle adjustments is {:.5f} degrees (allowed range is {} "
                         "<= delta <= {}).\n",
                         angleStepSize_, angleStepSizeMin_, angleStepSizeMax_);
    if (adjustTorsions_)
        Messenger::print("IntraShake: Rotation step size for torsion adjustments is {:.5f} degrees (allowed range "
                         "is {} <= delta <= {}).\n",
                         torsionStepSize_, torsionStepSizeMin_, torsionStepSizeMax_);
    Messenger::print("IntraShake: Target acceptance rate is {}.\n", targetAcceptanceRate_);
    if (!restrictToSpecies_.empty())
        Messenger::print("IntraShake: Calculation will be restricted to species: {}\n",
                         joinStrings(restrictToSpecies_, "  ", [](const auto &sp) { return sp->name(); }));

    Messenger::print("\n");

    ProcessPool::DivisionStrategy strategy = moduleContext.processPool().bestStrategy();
    Timer commsTimer(false);

    // Create a Molecule distributor
    RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells(),
                                    moduleContext.processPool(), strategy);

    // Create a local ChangeStore and EnergyKernel
    ChangeStore changeStore(moduleContext.processPool(), commsTimer);
    auto kernel = KernelProducer::energyKernel(targetConfiguration_, moduleContext.processPool(),
                                               moduleContext.dissolve().potentialMap(), rCut);

    // Initialise the random number buffer
    RandomBuffer randomBuffer(moduleContext.processPool(), ProcessPool::subDivisionStrategy(strategy), commsTimer);

    // Determine target molecules from the restrictedSpecies vector (if any) and give to the distributor
    if (!restrictToSpecies_.empty())
    {
        std::vector<int> targetIndices;
        auto id = 0;
        for (const auto &mol : targetConfiguration_->molecules())
        {
            if (std::find(restrictToSpecies_.begin(), restrictToSpecies_.end(), mol->species()) != restrictToSpecies_.end())
                targetIndices.push_back(id);
            ++id;
        }
        distributor.setTargetMolecules(targetIndices);
    }

    // Ensure that the Species used in the present Configuration have attached atom lists
    for (auto &spPop : targetConfiguration_->speciesPopulations())
    {
        if (!spPop.first->attachedAtomListsGenerated())
        {
            Messenger::error("Species '{}' has no attached atom lists, so module can't proceed.\n", spPop.first->name());
            return ExecutionResult::Failed;
        }
    }

    auto shake = 0, nBondAttempts = 0, nAngleAttempts = 0, nTorsionAttempts = 0, nBondAccepted = 0, nAngleAccepted = 0,
         nTorsionAccepted = 0;
    auto totalDelta = 0.0;
    Matrix3 transform;
    const auto *box = targetConfiguration_->box();

    Timer timer;
    while (distributor.cycle())
    {
        // Get next set of Molecule targets from the distributor
        std::vector<int> targetMolecules = distributor.assignedMolecules();

        // Switch parallel strategy if necessary
        if (distributor.currentStrategy() != strategy)
        {
            // Set the new strategy
            strategy = distributor.currentStrategy();

            // Re-initialise the random buffer
            randomBuffer.reset(ProcessPool::subDivisionStrategy(strategy));
        }

        // Loop over target Molecule
        for (auto molId : targetMolecules)
        {
            /*
             * Calculation Begins
             */

            // Get Molecule index and pointer
            std::shared_ptr<Molecule> mol = targetConfiguration_->molecule(molId);
            const auto indexOffset = mol->globalAtomOffset();

            // Set current atom targets in ChangeStore (whole molecule)
            changeStore.add(mol);

            // Calculate starting reference energy for Molecule
            auto referenceEnergy = kernel->totalEnergy(*mol);

            // Loop over defined bonds
            if (adjustBonds_)
                for (const auto &bond : mol->species()->bonds())
                {
                    // Get Atom pointers
                    auto i = mol->atom(bond.indexI());
                    auto j = mol->atom(bond.indexJ());

                    // Select random terminus
                    auto terminus = randomBuffer.random() > 0.5 ? 1 : 0;

                    // Loop over number of shakes per term
                    for (shake = 0; shake < nShakesPerTerm_; ++shake)
                    {
                        // Get translation vector, normalise, and apply random delta
                        auto vji = box->minimumVector(i->r(), j->r());
                        vji.normalise();
                        vji *= randomBuffer.randomPlusMinusOne() * bondStepSize_;

                        // Adjust the Atoms attached to the selected terminus
                        mol->translate(vji, bond.attachedAtoms(terminus));

                        // Update Cell positions of the adjusted Atoms
                        targetConfiguration_->updateAtomLocations(bond.attachedAtoms(terminus), indexOffset);

                        // Calculate new energy and delta
                        auto newEnergy = kernel->totalEnergy(*mol);
                        auto delta = newEnergy.total() - referenceEnergy.total();

                        // Accept new (current) positions of the Molecule's Atoms?
                        if (delta < 0 || (randomBuffer.random() < exp(-delta * rRT)))
                        {
                            changeStore.updateAll();
                            referenceEnergy = newEnergy;
                            distributor.increase(totalDelta, delta);
                            distributor.increment(nBondAccepted);
                        }
                        else
                            changeStore.revertAll();

                        distributor.increment(nBondAttempts);
                    }
                }

            // Loop over defined angles
            if (adjustAngles_)
                for (const auto &angle : mol->species()->angles())
                {
                    // Get Atom pointers
                    auto i = mol->atom(angle.indexI());
                    auto j = mol->atom(angle.indexJ());
                    auto k = mol->atom(angle.indexK());

                    // Select random terminus
                    auto terminus = randomBuffer.random() > 0.5 ? 1 : 0;

                    // Loop over number of shakes per term
                    for (shake = 0; shake < nShakesPerTerm_; ++shake)
                    {
                        // Get bond vectors and calculate cross product to get rotation axis
                        auto vji = box->minimumVector(j->r(), i->r());
                        auto vjk = box->minimumVector(j->r(), k->r());
                        auto v = vji * vjk;

                        // Create suitable transformation matrix
                        transform.createRotationAxis(v, randomBuffer.randomPlusMinusOne() * angleStepSize_, true);

                        // Adjust the Atoms attached to the selected terminus
                        mol->transform(box, transform, j->r(), angle.attachedAtoms(terminus));

                        // Update Cell positions of the adjusted Atoms
                        targetConfiguration_->updateAtomLocations(angle.attachedAtoms(terminus), indexOffset);

                        // Calculate new energy and delta
                        auto newEnergy = kernel->totalEnergy(*mol);
                        auto delta = newEnergy.total() - referenceEnergy.total();

                        // Accept new (current) positions of the Molecule's Atoms?
                        if (delta < 0 || (randomBuffer.random() < exp(-delta * rRT)))
                        {
                            changeStore.updateAll();
                            referenceEnergy = newEnergy;
                            distributor.increase(totalDelta, delta);
                            distributor.increment(nAngleAccepted);
                        }
                        else
                            changeStore.revertAll();

                        distributor.increment(nAngleAttempts);
                    }
                }

            // Loop over defined torsions
            if (adjustTorsions_)
                for (const auto &torsion : mol->species()->torsions())
                {
                    // Refuse to change a torsion which is in a cycle
                    if (torsion.inCycle())
                        continue;

                    // Get Atom pointers
                    auto j = mol->atom(torsion.indexJ());
                    auto k = mol->atom(torsion.indexK());

                    // Select random terminus
                    auto terminus = randomBuffer.random() > 0.5 ? 1 : 0;

                    // Loop over number of shakes per term
                    for (shake = 0; shake < nShakesPerTerm_; ++shake)
                    {
                        // Get bond vectors j-k to get rotation axis
                        auto vjk = box->minimumVector(j->r(), k->r());

                        // Create suitable transformation matrix
                        transform.createRotationAxis(vjk, randomBuffer.randomPlusMinusOne() * torsionStepSize_, true);

                        // Adjust the Atoms attached to the selected terminus
                        mol->transform(box, transform, terminus == 0 ? j->r() : k->r(), torsion.attachedAtoms(terminus));

                        // Update Cell positions of the adjusted Atoms
                        targetConfiguration_->updateAtomLocations(torsion.attachedAtoms(terminus), indexOffset);

                        // Calculate new energy and delta
                        auto newEnergy = kernel->totalEnergy(*mol);
                        auto delta = newEnergy.total() - referenceEnergy.total();

                        // Accept new (current) positions of the Molecule's Atoms?
                        if (delta < 0 || (randomBuffer.random() < exp(-delta * rRT)))
                        {
                            changeStore.updateAll();
                            referenceEnergy = newEnergy;
                            distributor.increase(totalDelta, delta);
                            distributor.increment(nTorsionAccepted);
                        }
                        else
                            changeStore.revertAll();

                        distributor.increment(nTorsionAttempts);
                    }
                }

            // Store modifications to Atom positions ready for broadcast
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
    if (!moduleContext.processPool().allSum(&totalDelta, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nBondAttempts, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nBondAccepted, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nAngleAttempts, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nAngleAccepted, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nTorsionAttempts, 1, strategy, commsTimer))
        return ExecutionResult::Failed;
    if (!moduleContext.processPool().allSum(&nTorsionAccepted, 1, strategy, commsTimer))
        return ExecutionResult::Failed;

    Messenger::print("IntraShake: Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);
    Messenger::print("IntraShake: Total number of attempted moves was {} ({} work, {} comms).\n",
                     nBondAttempts + nAngleAttempts + nTorsionAttempts, timer.totalTimeString(), commsTimer.totalTimeString());

    // Calculate and report acceptance rates and adjust step sizes - if no moves were accepted, just decrease the
    // current stepSize by a constant factor
    if (adjustBonds_ && (nBondAttempts > 0))
    {
        double bondRate = double(nBondAccepted) / nBondAttempts;
        Messenger::print("IntraShake: Overall bond shake acceptance rate was {:4.2f}% ({} of {} attempted moves).\n",
                         100.0 * bondRate, nBondAccepted, nBondAttempts);

        bondStepSize_ *= (nBondAccepted == 0) ? 0.8 : bondRate / targetAcceptanceRate_;
        if (bondStepSize_ < bondStepSizeMin_)
            bondStepSize_ = bondStepSizeMin_;
        else if (bondStepSize_ > bondStepSizeMax_)
            bondStepSize_ = bondStepSizeMax_;
        keywords_.set("BondStepSize", bondStepSize_);

        Messenger::print("IntraShake: Updated distance step size for bond adjustments is {:.5f} Angstroms.\n", bondStepSize_);
    }

    if (adjustAngles_ && (nAngleAttempts > 0))
    {
        double angleRate = double(nAngleAccepted) / nAngleAttempts;
        Messenger::print("IntraShake: Overall angle shake acceptance rate was {:4.2f}% ({} of {} attempted moves).\n",
                         100.0 * angleRate, nAngleAccepted, nAngleAttempts);

        angleStepSize_ *= (nAngleAccepted == 0) ? 0.8 : angleRate / targetAcceptanceRate_;
        if (angleStepSize_ < angleStepSizeMin_)
            angleStepSize_ = angleStepSizeMin_;
        else if (angleStepSize_ > angleStepSizeMax_)
            angleStepSize_ = angleStepSizeMax_;
        keywords_.set("AngleStepSize", angleStepSize_);

        Messenger::print("IntraShake: Updated rotation step size for angle adjustments is {:.5f} degrees.\n", angleStepSize_);
    }

    if (adjustTorsions_ && (nTorsionAttempts > 0))
    {
        double torsionRate = double(nTorsionAccepted) / nTorsionAttempts;
        Messenger::print("IntraShake: Overall torsion shake acceptance rate was {:4.2f}% ({} of {} attempted moves).\n",
                         100.0 * torsionRate, nTorsionAccepted, nTorsionAttempts);

        torsionStepSize_ *= (nTorsionAccepted == 0) ? 0.8 : torsionRate / targetAcceptanceRate_;
        if (torsionStepSize_ < torsionStepSizeMin_)
            torsionStepSize_ = torsionStepSizeMin_;
        else if (torsionStepSize_ > torsionStepSizeMax_)
            torsionStepSize_ = torsionStepSizeMax_;
        keywords_.set("TorsionStepSize", torsionStepSize_);

        Messenger::print("IntraShake: Updated rotation step size for torsion adjustments is {:.5f} degrees.\n",
                         torsionStepSize_);
    }

    // Increase contents version in Configuration
    if ((nBondAccepted > 0) || (nAngleAccepted > 0) || (nTorsionAccepted > 0))
        targetConfiguration_->incrementContentsVersion();

    return ExecutionResult::Success;
}
