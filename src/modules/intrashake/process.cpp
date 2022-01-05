// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "classes/regionaldistributor.h"
#include "classes/species.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "main/dissolve.h"
#include "modules/intrashake/intrashake.h"

// Run main processing
bool IntraShakeModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Perform intramolecular shakes, modifying individual bound terms on molecules.
     *
     * This is a parallel routine, with processes operating as one pool
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (auto *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters
        auto rCut = cutoffDistance_.value_or(dissolve.pairPotentialRange());
        const auto rRT = 1.0 / (.008314472 * cfg->temperature());

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
        if (termEnergyOnly_)
            Messenger::print("IntraShake: Only term energy will be considered (interatomic contributions with the "
                             "system will be excluded).\n");
        Messenger::print("\n");

        ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();

        // Create a Molecule distributor
        RegionalDistributor distributor(cfg->nMolecules(), cfg->cells(), procPool, strategy);

        // Create a local ChangeStore and EnergyKernel
        ChangeStore changeStore(procPool);
        EnergyKernel kernel(procPool, cfg->box(), cfg->cells(), dissolve.potentialMap(), rCut);

        // Initialise the random number buffer
        procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));

        // Ensure that the Species used in the present Configuration have attached atom lists
        for (auto &spPop : cfg->speciesPopulations())
        {
            if (!spPop.first->attachedAtomListsGenerated())
                return Messenger::error("Species '{}' has no attached atom lists, so module can't proceed.\n",
                                        spPop.first->name());
        }

        int shake, nBondAttempts = 0, nAngleAttempts = 0, nTorsionAttempts = 0, nBondAccepted = 0, nAngleAccepted = 0,
                   nTorsionAccepted = 0;
        int terminus;
        bool accept;
        double ppEnergy, newPPEnergy, intraEnergy, newIntraEnergy, delta, totalDelta = 0.0;
        Vec3<double> vji, vjk, v;
        Matrix3 transform;
        const auto *box = cfg->box();
        Atom *i, *j, *k, *l;

        Timer timer;
        procPool.resetAccumulatedTime();
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
                procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));
            }

            // Loop over target Molecule
            for (auto molId : targetMolecules)
            {
                /*
                 * Calculation Begins
                 */

                // Get Molecule index and pointer
                std::shared_ptr<Molecule> mol = cfg->molecule(molId);
                const auto indexOffset = mol->atom(0)->arrayIndex();

                // Set current atom targets in ChangeStore (whole molecule)
                changeStore.add(mol);

                // Calculate reference pairpotential energy for Molecule
                ppEnergy = termEnergyOnly_ ? 0.0 : kernel.energy(*mol, ProcessPool::subDivisionStrategy(strategy));

                // Loop over defined bonds
                if (adjustBonds_)
                    for (const auto &bond : mol->species()->bonds())
                    {
                        // Get Atom pointers
                        i = mol->atom(bond.indexI());
                        j = mol->atom(bond.indexJ());

                        // Store current energy of this intramolecular term, or the whole Molecule if it
                        // is present in a cycle
                        intraEnergy = bond.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(bond, *i, *j);

                        // Select random terminus
                        terminus = procPool.random() > 0.5 ? 1 : 0;

                        // Loop over number of shakes per term
                        for (shake = 0; shake < nShakesPerTerm_; ++shake)
                        {
                            // Get translation vector, normalise, and apply random delta
                            vji = box->minimumVector(i->r(), j->r());
                            vji.normalise();
                            vji *= procPool.randomPlusMinusOne() * bondStepSize_;

                            // Adjust the Atoms attached to the selected terminus
                            mol->translate(vji, bond.attachedAtoms(terminus));

                            // Update Cell positions of the adjusted Atoms
                            cfg->updateCellLocation(bond.attachedAtoms(terminus), indexOffset);

                            // Calculate new energy
                            newPPEnergy =
                                termEnergyOnly_ ? 0.0 : kernel.energy(*mol, ProcessPool::subDivisionStrategy(strategy));
                            newIntraEnergy = bond.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(bond, *i, *j);

                            // Trial the transformed Molecule
                            delta = (newPPEnergy + newIntraEnergy) - (ppEnergy + intraEnergy);
                            accept = delta < 0 ? true : (procPool.random() < exp(-delta * rRT));

                            // Accept new (current) positions of the Molecule's Atoms?
                            if (accept)
                            {
                                changeStore.updateAll();
                                ppEnergy = newPPEnergy;
                                intraEnergy = newIntraEnergy;
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
                        i = mol->atom(angle.indexI());
                        j = mol->atom(angle.indexJ());
                        k = mol->atom(angle.indexK());

                        // Store current energy of this intramolecular term
                        intraEnergy = angle.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(angle, *i, *j, *k);

                        // Select random terminus
                        terminus = procPool.random() > 0.5 ? 1 : 0;

                        // Loop over number of shakes per term
                        for (shake = 0; shake < nShakesPerTerm_; ++shake)
                        {
                            // Get bond vectors and calculate cross product to get rotation axis
                            vji = box->minimumVector(j->r(), i->r());
                            vjk = box->minimumVector(j->r(), k->r());
                            v = vji * vjk;

                            // Create suitable transformation matrix
                            transform.createRotationAxis(v.x, v.y, v.z, procPool.randomPlusMinusOne() * angleStepSize_, true);

                            // Adjust the Atoms attached to the selected terminus
                            mol->transform(box, transform, angle.j()->r(), angle.attachedAtoms(terminus));

                            // Update Cell positions of the adjusted Atoms
                            cfg->updateCellLocation(angle.attachedAtoms(terminus), indexOffset);

                            // Calculate new energy
                            newPPEnergy =
                                termEnergyOnly_ ? 0.0 : kernel.energy(*mol, ProcessPool::subDivisionStrategy(strategy));
                            newIntraEnergy =
                                angle.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(angle, *i, *j, *k);

                            // Trial the transformed Molecule
                            delta = (newPPEnergy + newIntraEnergy) - (ppEnergy + intraEnergy);
                            accept = delta < 0 ? true : (procPool.random() < exp(-delta * rRT));

                            // Accept new (current) positions of the Molecule's Atoms?
                            if (accept)
                            {
                                changeStore.updateAll();
                                ppEnergy = newPPEnergy;
                                intraEnergy = newIntraEnergy;
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
                        // Get Atom pointers
                        i = mol->atom(torsion.indexI());
                        j = mol->atom(torsion.indexJ());
                        k = mol->atom(torsion.indexK());
                        l = mol->atom(torsion.indexL());

                        // Store current energy of this intramolecular term
                        intraEnergy =
                            torsion.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(torsion, *i, *j, *k, *l);

                        // Select random terminus
                        terminus = procPool.random() > 0.5 ? 1 : 0;

                        // Loop over number of shakes per term
                        for (shake = 0; shake < nShakesPerTerm_; ++shake)
                        {
                            // Get bond vectors j-k to get rotation axis
                            vjk = box->minimumVector(j->r(), k->r());

                            // Create suitable transformation matrix
                            transform.createRotationAxis(vjk.x, vjk.y, vjk.z, procPool.randomPlusMinusOne() * torsionStepSize_,
                                                         true);

                            // Adjust the Atoms attached to the selected terminus
                            mol->transform(box, transform, terminus == 0 ? j->r() : k->r(), torsion.attachedAtoms(terminus));

                            // Update Cell positions of the adjusted Atoms
                            cfg->updateCellLocation(torsion.attachedAtoms(terminus), indexOffset);

                            // Calculate new energy
                            newPPEnergy =
                                termEnergyOnly_ ? 0.0 : kernel.energy(*mol, ProcessPool::subDivisionStrategy(strategy));
                            newIntraEnergy =
                                torsion.inCycle() ? kernel.intramolecularEnergy(*mol) : kernel.energy(torsion, *i, *j, *k, *l);

                            // Trial the transformed Molecule
                            delta = (newPPEnergy + newIntraEnergy) - (ppEnergy + intraEnergy);
                            accept = delta < 0 ? true : (procPool.random() < exp(-delta * rRT));

                            // Accept new (current) positions of the Molecule's Atoms?
                            if (accept)
                            {
                                changeStore.updateAll();
                                ppEnergy = newPPEnergy;
                                intraEnergy = newIntraEnergy;
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
            changeStore.distributeAndApply(cfg);
            changeStore.reset();
        }
        timer.stop();

        // Collect statistics across all processe
        if (!procPool.allSum(&totalDelta, 1, strategy))
            return false;
        if (!procPool.allSum(&nBondAttempts, 1, strategy))
            return false;
        if (!procPool.allSum(&nBondAccepted, 1, strategy))
            return false;
        if (!procPool.allSum(&nAngleAttempts, 1, strategy))
            return false;
        if (!procPool.allSum(&nAngleAccepted, 1, strategy))
            return false;
        if (!procPool.allSum(&nTorsionAttempts, 1, strategy))
            return false;
        if (!procPool.allSum(&nTorsionAccepted, 1, strategy))
            return false;

        Messenger::print("IntraShake: Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);
        Messenger::print("IntraShake: Total number of attempted moves was {} ({} work, {} comms).\n",
                         nBondAttempts + nAngleAttempts + nTorsionAttempts, timer.totalTimeString(),
                         procPool.accumulatedTimeString());

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

            Messenger::print("IntraShake: Updated distance step size for bond adjustments is {:.5f} Angstroms.\n",
                             bondStepSize_);
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

            Messenger::print("IntraShake: Updated rotation step size for angle adjustments is {:.5f} degrees.\n",
                             angleStepSize_);
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
            cfg->incrementContentsVersion();
    }

    return true;
}
