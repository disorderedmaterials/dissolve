/*
    *** MolShake Module - Processing
    *** src/modules/molshake/process.cpp
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
#include "base/timer.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/regionaldistributor.h"
#include "classes/scaledenergykernel.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/molshake/molshake.h"

// Run main processing
bool MolShakeModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Perform a Molecule shake
     *
     * This is a parallel routine, with processes operating as groups.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (RefListItem<Configuration> *ri = targetConfigurations_.first(); ri != nullptr; ri = ri->next())
    {
        // Grab Configuration pointer
        Configuration *cfg = ri->item();

        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters from Configuration
        double cutoffDistance = keywords_.asDouble("CutoffDistance");
        if (cutoffDistance < 0.0)
            cutoffDistance = dissolve.pairPotentialRange();
        auto &rotationStepSize = keywords_.retrieve<double>("RotationStepSize");
        const auto rotationStepSizeMax = keywords_.asDouble("RotationStepSizeMax");
        const auto rotationStepSizeMin = keywords_.asDouble("RotationStepSizeMin");
        const auto nShakesPerMolecule = keywords_.asInt("ShakesPerMolecule");
        const auto targetAcceptanceRate = keywords_.asDouble("TargetAcceptanceRate");
        auto &translationStepSize = keywords_.retrieve<double>("TranslationStepSize");
        const auto translationStepSizeMax = keywords_.asDouble("TranslationStepSizeMax");
        const auto translationStepSizeMin = keywords_.asDouble("TranslationStepSizeMin");
        const auto rRT = 1.0 / (.008314472 * cfg->temperature());

        // Print argument/parameter summary
        Messenger::print("MolShake: Cutoff distance is {}.\n", cutoffDistance);
        Messenger::print("MolShake: Performing {} shake(s) per Molecule.\n", nShakesPerMolecule);
        Messenger::print("MolShake: Step size for translation adjustments is {:.5f} Angstroms (allowed range is {} <= "
                         "delta <= {}).\n",
                         translationStepSize, translationStepSizeMin, translationStepSizeMax);
        Messenger::print(
            "MolShake: Step size for rotation adjustments is {:.5f} degrees (allowed range is {} <= delta <= {}).\n",
            rotationStepSize, rotationStepSizeMin, rotationStepSizeMax);
        Messenger::print("\n");

        ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();

        // Create a Molecule distributor
        std::deque<std::shared_ptr<Molecule>> &moleculeArray = cfg->molecules();
        RegionalDistributor distributor(moleculeArray, cfg->cells(), procPool, strategy);

        // Create a local ChangeStore and a suitable EnergyKernel
        ChangeStore changeStore(procPool);
        EnergyKernel kernel(procPool, cfg, dissolve.potentialMap(), cutoffDistance);

        // Initialise the random number buffer
        procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));

        int shake, nRotationAttempts = 0, nTranslationAttempts = 0, nRotationsAccepted = 0, nTranslationsAccepted = 0,
                   nGeneralAttempts = 0;
        bool accept;
        double currentEnergy, newEnergy, delta, totalDelta = 0.0;
        Matrix3 transform;
        Vec3<double> rDelta;
        const Box *box = cfg->box();

        /*
         * In order to be able to adjust translation and rotational steps independently, we will perform 80% of moves
         * including both a translation a rotation, 10% using only translations, and 10% using only rotations.
         */

        // Set initial random offset for our counter determining whether to perform R+T, R, or T.
        auto count = procPool.random() * 10;
        bool rotate, translate;

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

            // Loop over target Molecules
            for (auto molId : targetMolecules)
            {
                /*
                 * Calculation Begins
                 */

                // Get Molecule index and pointer
                std::shared_ptr<Molecule> mol = cfg->molecule(molId);

                // Set current atom targets in ChangeStore (whole Molecule)
                changeStore.add(mol);

                // Calculate reference energy for Molecule, including intramolecular terms
                currentEnergy = kernel.energy(mol, ProcessPool::subDivisionStrategy(strategy), true);

                // Loop over number of shakes per atom
                for (shake = 0; shake < nShakesPerMolecule; ++shake)
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
                        rDelta.set(procPool.randomPlusMinusOne() * translationStepSize,
                                   procPool.randomPlusMinusOne() * translationStepSize,
                                   procPool.randomPlusMinusOne() * translationStepSize);
                        mol->translate(rDelta);
                    }

                    // Create a random rotation matrix and apply it to the Molecule
                    if (rotate)
                    {
                        transform.createRotationXY(procPool.randomPlusMinusOne() * rotationStepSize,
                                                   procPool.randomPlusMinusOne() * rotationStepSize);
                        mol->transform(box, transform);
                    }

                    // Update Cell positions of Atoms in the Molecule
                    cfg->updateCellLocation(mol);

                    // Calculate new energy
                    newEnergy = kernel.energy(mol, ProcessPool::subDivisionStrategy(strategy), true);

                    // Trial the transformed atom position
                    delta = newEnergy - currentEnergy;
                    accept = delta < 0 ? true : (procPool.random() < exp(-delta * rRT));

                    if (accept)
                    {
                        // Accept new (current) position of target Atoms
                        changeStore.updateAll();
                        currentEnergy = newEnergy;
                    }
                    else
                        changeStore.revertAll();

                    // Increase attempt counters
                    // The strategy in force at any one time may vary, so use the distributor's helper
                    // functions.
                    if (distributor.collectStatistics())
                    {
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
                    }

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
            changeStore.distributeAndApply(cfg);
            changeStore.reset();
        }

        // Collect statistics across all processes
        if (!procPool.allSum(&totalDelta, 1))
            return false;
        if (!procPool.allSum(&nGeneralAttempts, 1))
            return false;
        if (!procPool.allSum(&nTranslationAttempts, 1))
            return false;
        if (!procPool.allSum(&nTranslationsAccepted, 1))
            return false;
        if (!procPool.allSum(&nRotationAttempts, 1))
            return false;
        if (!procPool.allSum(&nRotationsAccepted, 1))
            return false;

        timer.stop();

        Messenger::print("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

        // Calculate and print acceptance rates
        double transRate = double(nTranslationsAccepted) / nTranslationAttempts;
        double rotRate = double(nRotationsAccepted) / nRotationAttempts;
        Messenger::print("Total number of attempted moves was {} ({} work, {} comms)\n", nGeneralAttempts,
                         timer.totalTimeString(), procPool.accumulatedTimeString());
        Messenger::print("Overall translation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * transRate,
                         nTranslationsAccepted, nTranslationAttempts);
        Messenger::print("Overall rotation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rotRate,
                         nRotationsAccepted, nRotationAttempts);

        // Update and set translation step size
        translationStepSize *= (nTranslationsAccepted == 0) ? 0.8 : transRate / targetAcceptanceRate;
        if (translationStepSize < translationStepSizeMin)
            translationStepSize = translationStepSizeMin;
        else if (translationStepSize > translationStepSizeMax)
            translationStepSize = translationStepSizeMax;

        Messenger::print("Updated step size for translations is {:.5f} Angstroms.\n", translationStepSize);

        // Update and set rotation step size
        rotationStepSize *= (nRotationsAccepted == 0) ? 0.8 : rotRate / targetAcceptanceRate;
        if (rotationStepSize < rotationStepSizeMin)
            rotationStepSize = rotationStepSizeMin;
        else if (rotationStepSize > rotationStepSizeMax)
            rotationStepSize = rotationStepSizeMax;

        Messenger::print("Updated step size for rotations is {:.5f} degrees.\n", rotationStepSize);

        // Increase contents version in Configuration
        if ((nRotationsAccepted > 0) || (nTranslationsAccepted > 0))
            cfg->incrementContentsVersion();
    }

    return true;
}
