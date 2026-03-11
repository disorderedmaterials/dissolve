// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/timer.h"
#include "classes/box.h"
#include "classes/changeStore.h"
#include "classes/regionalDistributor.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include "modules/molShake/molShake.h"

// Run main processing
Module::ExecutionResult MolShakeModule::process(Dissolve &dissolve)
{
    // Retrieve control parameters from Configuration
    auto rCut = cutoffDistance_.value_or(PairPotential::range());
    const auto rRT = 1.0 / (.008314472 * targetConfiguration_->temperature());

    // Print argument/parameter summary
    Messenger::print("MolShake: Cutoff distance is {}.\n", rCut);
    Messenger::print("MolShake: Performing {} shake(s) per Molecule.\n", nShakesPerMolecule_);
    Messenger::print("MolShake: Step size for translation adjustments is {:.5f} Angstroms (allowed range is {} <= "
                     "delta <= {}).\n",
                     translationStepSize_, translationStepSizeMin_, translationStepSizeMax_);
    Messenger::print("MolShake: Step size for rotation adjustments is {:.5f} degrees (allowed range is {} <= delta <= {}).\n",
                     rotationStepSize_, rotationStepSizeMin_, rotationStepSizeMax_);
    if (!restrictToSpecies_.empty())
        Messenger::print("MolShake: Calculation will be restricted to species: {}\n",
                         joinStrings(restrictToSpecies_, "  ", [](const auto &sp) { return sp->name(); }));
    Messenger::print("\n");

    // Create a Molecule distributor
    RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells());

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

    // Create a local ChangeStore and a suitable EnergyKernel
    ChangeStore changeStore;
    auto kernel = KernelProducer::energyKernel(targetConfiguration_, dissolve.potentialMap());

    int shake, nRotationAttempts = 0, nTranslationAttempts = 0, nRotationsAccepted = 0, nTranslationsAccepted = 0,
               nGeneralAttempts = 0;
    bool accept;
    double currentEnergy, newEnergy, delta, totalDelta = 0.0;
    Matrix3 transform;
    Vector3 rDelta;
    const auto *box = targetConfiguration_->box();

    /*
     * In order to be able to adjust translation and rotational steps independently, we will perform 80% of moves
     * including both a translation a rotation, 10% using only translations, and 10% using only rotations.
     */

    // Set initial random offset for our counter determining whether to perform R+T, R, or T.
    auto count = DissolveMath::random() * 10;
    bool rotate, translate;

    Timer timer;
    while (distributor.cycle())
    {
        // Get next set of Molecule targets from the distributor
        auto &targetIndices = distributor.assignedMolecules();

        // Loop over target Molecules
        for (auto molId : targetIndices)
        {
            /*
             * Calculation Begins
             */

            // Get Molecule index and pointer
            auto mol = targetConfiguration_->molecule(molId);

            // Set current atom targets in ChangeStore (whole Molecule)
            changeStore.add(mol);

            // Calculate reference pair potential energy for Molecule, excluding all intramolecular contributions
            currentEnergy =
                kernel->totalEnergy(*mol, {Kernel::ExcludeGeometric, Kernel::ExcludeIntraMolecularPairPotential}).total();

            // Loop over number of shakes per atom
            for (shake = 0; shake < nShakesPerMolecule_; ++shake)
            {
                // Determine what move(s) will we attempt
                if (count == 0)
                {
                    rotate = true;
                    translate = false;
                }
                else if (count == 1)
                {
                    rotate = false;
                    translate = true;
                }
                else
                {
                    rotate = true;
                    translate = true;
                }

                // Create a random translation vector and apply it to the Molecule's centre
                if (translate)
                {
                    rDelta.set(DissolveMath::randomPlusMinusOne() * translationStepSize_,
                               DissolveMath::randomPlusMinusOne() * translationStepSize_,
                               DissolveMath::randomPlusMinusOne() * translationStepSize_);
                    mol->translate(rDelta);
                }

                // Create a random rotation matrix and apply it to the Molecule
                if (rotate)
                {
                    transform.createRotationXY(DissolveMath::randomPlusMinusOne() * rotationStepSize_,
                                               DissolveMath::randomPlusMinusOne() * rotationStepSize_);
                    mol->transform(box, transform);
                }

                // Update Cell positions of Atoms in the Molecule
                targetConfiguration_->updateAtomLocations(mol);

                // Calculate new energy
                newEnergy =
                    kernel->totalEnergy(*mol, {Kernel::ExcludeGeometric, Kernel::ExcludeIntraMolecularPairPotential}).total();

                // Trial the transformed atom position
                delta = newEnergy - currentEnergy;
                accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));

                if (accept)
                {
                    // Accept new (current) position of target Atoms
                    changeStore.updateAll();
                    currentEnergy = newEnergy;
                }
                else
                    changeStore.revertAll();

                // Increase attempt counters
                if (accept)
                    totalDelta += delta;
                if (rotate)
                {
                    if (accept)
                        ++nRotationsAccepted;
                    ++nRotationAttempts;
                }
                if (translate)
                {
                    if (accept)
                        ++nTranslationsAccepted;
                    ++nTranslationAttempts;
                }
                ++nGeneralAttempts;

                // Increase and fold move type counter
                ++count;
                if (count > 9)
                    count = 0;
            }

            // Store modifications to Atom positions ready for broadcast
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

    // Calculate and print acceptance rates
    double transRate = double(nTranslationsAccepted) / nTranslationAttempts;
    double rotRate = double(nRotationsAccepted) / nRotationAttempts;
    Messenger::print("Total number of attempted moves was {} ({})\n", nGeneralAttempts, timer.totalTimeString());
    Messenger::print("Overall translation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * transRate,
                     nTranslationsAccepted, nTranslationAttempts);
    Messenger::print("Overall rotation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rotRate,
                     nRotationsAccepted, nRotationAttempts);

    // Update and set translation step size
    translationStepSize_ *= (nTranslationsAccepted == 0) ? 0.8 : transRate / targetAcceptanceRate_;
    if (translationStepSize_ < translationStepSizeMin_)
        translationStepSize_ = translationStepSizeMin_;
    else if (translationStepSize_ > translationStepSizeMax_)
        translationStepSize_ = translationStepSizeMax_;
    keywords_.set("TranslationStepSize", translationStepSize_);

    Messenger::print("Updated step size for translations is {:.5f} Angstroms.\n", translationStepSize_);

    // Update and set rotation step size
    rotationStepSize_ *= (nRotationsAccepted == 0) ? 0.8 : rotRate / targetAcceptanceRate_;
    if (rotationStepSize_ < rotationStepSizeMin_)
        rotationStepSize_ = rotationStepSizeMin_;
    else if (rotationStepSize_ > rotationStepSizeMax_)
        rotationStepSize_ = rotationStepSizeMax_;
    keywords_.set("RotationStepSize", rotationStepSize_);

    Messenger::print("Updated step size for rotations is {:.5f} degrees.\n", rotationStepSize_);

    // Increase contents version in Configuration
    if ((nRotationsAccepted > 0) || (nTranslationsAccepted > 0))
        targetConfiguration_->notifyAtomicPositionsChanged();

    return ExecutionResult::Success;
}
