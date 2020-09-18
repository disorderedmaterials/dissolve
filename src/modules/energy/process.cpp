/*
    *** Energy Module - Processing
    *** src/modules/energy/process.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "modules/energy/energy.h"

// Run set-up stage
bool EnergyModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    // For each Configuration target, add a flag to its moduleData (which is *not* stored in the restart file) that we are
    // targeting it
    for (Configuration *cfg : targetConfigurations_)
        GenericListHelper<bool>::realise(cfg->moduleData(), "_IsEnergyModuleTarget", "", GenericItem::ProtectedFlag) = true;

    return true;
}

// Run main processing
bool EnergyModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate Energy for the target Configuration(s)
     *
     * This is a parallel routine, with processes operating in groups, unless in TEST mode.
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
        const bool saveData = keywords_.asBool("Save");
        const auto stabilityThreshold = keywords_.asDouble("StabilityThreshold");
        const auto stabilityWindow = keywords_.asInt("StabilityWindow");
        const bool testAnalytic = keywords_.asBool("TestAnalytic");
        const bool testMode = keywords_.asBool("Test");
        const auto testThreshold = keywords_.asDouble("TestThreshold");
        auto hasReferenceInter = keywords_.isSet("TestReferenceInter");
        const auto testReferenceInter = keywords_.asDouble("TestReferenceInter");
        auto hasReferenceIntra = keywords_.isSet("TestReferenceIntra");
        const auto testReferenceIntra = keywords_.asDouble("TestReferenceIntra");

        // Print parameter summary
        if (testMode)
        {
            Messenger::print("Energy: All energies will be calculated in serial test mode and compared to "
                             "production values.\n");
            if (testAnalytic)
                Messenger::print("Energy: Exact, analytical potential will be used in test.");
            if (hasReferenceInter)
                Messenger::print("Energy: Reference interatomic energy is {:15.9e} kJ/mol.\n", testReferenceInter);
            if (hasReferenceIntra)
                Messenger::print("Energy: Reference intramolecular energy is {:15.9e} kJ/mol.\n", testReferenceIntra);
        }

        Messenger::print("\n");

        Messenger::print("Calculating total energy for Configuration '{}'...\n", cfg->name());
        // Calculate the total energy
        if (testMode)
        {
            /*
             * Calculate the total energy of the system using a basic loop on each process, and then compare with
             * production routines.
             */

            /*
             * Test Calculation Begins
             */

            const PotentialMap &potentialMap = dissolve.potentialMap();
            double correctInterEnergy = 0.0, correctIntraEnergy = 0.0;

            double r, angle;
            Atom *i, *j;
            Vec3<double> vecji, vecjk, veckl;
            std::shared_ptr<Molecule> molN, molM;
            const Box *box = cfg->box();
            double scale;
            const auto cutoff = dissolve.potentialMap().range();

            Timer testTimer;

            // Calculate interatomic energy in a loop over defined Molecules
            for (int n = 0; n < cfg->nMolecules(); ++n)
            {
                molN = cfg->molecule(n);

                // Molecule self-energy
                for (int ii = 0; ii < molN->nAtoms() - 1; ++ii)
                {
                    i = molN->atom(ii);

                    // 					Messenger::print("Atom {} r = {} {} {}\n", ii,
                    // molN->atom(ii)->r().x, molN->atom(ii)->r().y, molN->atom(ii)->r().z);
                    for (int jj = ii + 1; jj < molN->nAtoms(); ++jj)
                    {
                        j = molN->atom(jj);

                        // Get interatomic distance
                        r = box->minimumDistance(i, j);
                        if (r > cutoff)
                            continue;

                        // Get intramolecular scaling of atom pair
                        scale = i->scaling(j);
                        if (scale < 1.0e-3)
                            continue;

                        if (testAnalytic)
                            correctInterEnergy += potentialMap.analyticEnergy(i, j, r) * scale;
                        else
                            correctInterEnergy += potentialMap.energy(i, j, r) * scale;
                    }
                }

                // Molecule-molecule energy
                for (int m = n + 1; m < cfg->nMolecules(); ++m)
                {
                    molM = cfg->molecule(m);

                    // Double loop over atoms
                    for (int ii = 0; ii < molN->nAtoms(); ++ii)
                    {
                        i = molN->atom(ii);

                        for (int jj = 0; jj < molM->nAtoms(); ++jj)
                        {
                            j = molM->atom(jj);

                            // Get interatomic distance and check cutoff
                            r = box->minimumDistance(i, j);
                            if (r > cutoff)
                                continue;

                            if (testAnalytic)
                                correctInterEnergy += potentialMap.analyticEnergy(i, j, r);
                            else
                                correctInterEnergy += potentialMap.energy(i, j, r);
                        }
                    }
                }

                // Bond energy
                for (const auto &bond : molN->species()->constBonds())
                {
                    r = cfg->box()->minimumDistance(molN->atom(bond.indexI()), molN->atom(bond.indexJ()));
                    correctIntraEnergy += bond.energy(r);
                }

                // Angle energy
                for (const auto &angle : molN->species()->constAngles())
                {
                    // Get vectors 'j-i' and 'j-k'
                    vecji = cfg->box()->minimumVector(molN->atom(angle.indexJ()), molN->atom(angle.indexI()));
                    vecjk = cfg->box()->minimumVector(molN->atom(angle.indexJ()), molN->atom(angle.indexK()));

                    // Calculate angle and determine angle energy
                    vecji.normalise();
                    vecjk.normalise();
                    correctIntraEnergy += angle.energy(Box::angleInDegrees(vecji, vecjk));
                }

                // Torsion energy
                for (const auto &torsion : molN->species()->constTorsions())
                {
                    // Get vectors 'j-i', 'j-k' and 'k-l'
                    vecji = cfg->box()->minimumVector(molN->atom(torsion.indexJ()), molN->atom(torsion.indexI()));
                    vecjk = cfg->box()->minimumVector(molN->atom(torsion.indexJ()), molN->atom(torsion.indexK()));
                    veckl = cfg->box()->minimumVector(molN->atom(torsion.indexK()), molN->atom(torsion.indexL()));

                    angle = Box::torsionInDegrees(vecji, vecjk, veckl);

                    // Determine Torsion energy
                    correctIntraEnergy += torsion.energy(angle);
                }

                // Improper energy
                for (const auto &imp : molN->species()->constImpropers())
                {
                    // Get vectors 'j-i', 'j-k' and 'k-l'
                    vecji = cfg->box()->minimumVector(molN->atom(imp.indexJ()), molN->atom(imp.indexI()));
                    vecjk = cfg->box()->minimumVector(molN->atom(imp.indexJ()), molN->atom(imp.indexK()));
                    veckl = cfg->box()->minimumVector(molN->atom(imp.indexK()), molN->atom(imp.indexL()));

                    angle = Box::torsionInDegrees(vecji, vecjk, veckl);

                    // Determine improper energy
                    correctIntraEnergy += imp.energy(angle);
                }
            }
            testTimer.stop();

            Messenger::print("Correct interatomic pairpotential energy is {:15.9e} kJ/mol\n", correctInterEnergy);
            Messenger::print("Correct intramolecular energy is {:15.9e} kJ/mol\n", correctIntraEnergy);
            Messenger::print("Correct total energy is {:15.9e} kJ/mol\n", correctInterEnergy + correctIntraEnergy);
            Messenger::print("Time to do total (test) energy was {}.\n", testTimer.totalTimeString());

            /*
             * Test Calculation End
             */

            /*
             * Production Calculation Begins
             */

            // Calculate interatomic energy
            Timer interTimer;
            double interEnergy = interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
            interTimer.stop();

            // Calculate intramolecular energy
            Timer intraTimer;
            double intraEnergy = intraMolecularEnergy(procPool, cfg, dissolve.potentialMap());
            intraTimer.stop();

            Messenger::print("Production interatomic pairpotential energy is {:15.9e} kJ/mol\n", interEnergy);
            Messenger::print("Production intramolecular energy is {:15.9e} kJ/mol\n", intraEnergy);
            Messenger::print("Total production energy is {:15.9e} kJ/mol\n", interEnergy + intraEnergy);
            Messenger::print("Time to do interatomic energy was {}.\n", interTimer.totalTimeString());
            Messenger::print("Time to do intramolecular energy was {}.\n", intraTimer.totalTimeString());

            /*
             * Production Calculation Ends
             */

            // Compare production vs reference values
            double delta;
            if (hasReferenceInter)
            {
                delta = testReferenceInter - correctInterEnergy;
                Messenger::print("Reference interatomic energy delta with correct value is {:15.9e} kJ/mol and "
                                 "is {} (threshold is {:10.3e} kJ/mol)\n",
                                 delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
                if (!procPool.allTrue(fabs(delta) < testThreshold))
                    return false;

                delta = testReferenceInter - interEnergy;
                Messenger::print("Reference interatomic energy delta with production value is {:15.9e} kJ/mol "
                                 "and is {} (threshold is {:10.3e} kJ/mol)\n",
                                 delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
                if (!procPool.allTrue(fabs(delta) < testThreshold))
                    return false;
            }
            if (hasReferenceIntra)
            {
                delta = testReferenceIntra - correctIntraEnergy;
                Messenger::print("Reference intramolecular energy delta with correct value is {:15.9e} kJ/mol "
                                 "and is {} (threshold is {:10.3e} kJ/mol)\n",
                                 delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
                if (!procPool.allTrue(fabs(delta) < testThreshold))
                    return false;

                delta = testReferenceIntra - intraEnergy;
                Messenger::print("Reference intramolecular energy delta with production value is {:15.9e} kJ/mol "
                                 "and is {} (threshold is {:10.3e} kJ/mol)\n",
                                 delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
                if (!procPool.allTrue(fabs(delta) < testThreshold))
                    return false;
            }

            // Compare production vs 'correct' values
            double interDelta = correctInterEnergy - interEnergy;
            double intraDelta = correctIntraEnergy - intraEnergy;
            Messenger::print("Comparing 'correct' with production values...\n");
            Messenger::print("Interatomic energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                             interDelta, fabs(interDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);
            Messenger::print("Intramolecular energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                             intraDelta, fabs(intraDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);

            // All OK?
            if (!procPool.allTrue((fabs(interDelta) < testThreshold) && (fabs(intraDelta) < testThreshold)))
                return false;
        }
        else
        {
            /*
             * Calculates the total energy of the entire system.
             *
             * This is a serial routine (subroutines called from within are parallel).
             */

            procPool.resetAccumulatedTime();

            // Calculate intermolecular energy
            Timer interTimer;
            double interEnergy = interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
            interTimer.stop();

            // Calculate intramolecular and intermolecular correction energy
            Timer intraTimer;
            double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;
            double intraEnergy = intraMolecularEnergy(procPool, cfg, dissolve.potentialMap(), bondEnergy, angleEnergy,
                                                      torsionEnergy, improperEnergy);
            intraTimer.stop();

            Messenger::print("Time to do interatomic energy was {}, intramolecular energy was {} ({} comms).\n",
                             interTimer.totalTimeString(), intraTimer.totalTimeString(), procPool.accumulatedTimeString());
            Messenger::print("Total Energy (World) is {:15.9e} kJ/mol ({:15.9e} kJ/mol interatomic + {:15.9e} kJ/mol "
                             "intramolecular).\n",
                             interEnergy + intraEnergy, interEnergy, intraEnergy);
            Messenger::print("Intramolecular contributions are - bonds = {:15.9e} kJ/mol, angles = {:15.9e} kJ/mol, "
                             "torsions = {:15.9e} kJ/mol.\n",
                             bondEnergy, angleEnergy, torsionEnergy);

            // Store current energies in the Configuration in case somebody else needs them
            auto &interData =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Inter", uniqueName(), GenericItem::InRestartFileFlag);
            interData.addPoint(dissolve.iteration(), interEnergy);
            interData.setObjectTag(fmt::format("{}//{}//Inter", cfg->niceName(), uniqueName()));
            auto &intraData =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Intra", uniqueName(), GenericItem::InRestartFileFlag);
            intraData.addPoint(dissolve.iteration(), intraEnergy);
            intraData.setObjectTag(fmt::format("{}//{}//Intra", cfg->niceName(), uniqueName()));
            auto &bondData =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Bond", uniqueName(), GenericItem::InRestartFileFlag);
            bondData.addPoint(dissolve.iteration(), bondEnergy);
            bondData.setObjectTag(fmt::format("{}//{}//Bond", cfg->niceName(), uniqueName()));
            auto &angleData =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Angle", uniqueName(), GenericItem::InRestartFileFlag);
            angleData.addPoint(dissolve.iteration(), angleEnergy);
            angleData.setObjectTag(fmt::format("{}//{}//Angle", cfg->niceName(), uniqueName()));
            auto &torsionData =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Torsion", uniqueName(), GenericItem::InRestartFileFlag);
            torsionData.addPoint(dissolve.iteration(), torsionEnergy);
            torsionData.setObjectTag(fmt::format("{}//{}//Torsion", cfg->niceName(), uniqueName()));

            // Append to arrays of total energies
            auto &totalEnergyArray =
                GenericListHelper<Data1D>::realise(cfg->moduleData(), "Total", uniqueName(), GenericItem::InRestartFileFlag);
            totalEnergyArray.addPoint(dissolve.iteration(), interEnergy + intraEnergy);
            totalEnergyArray.setObjectTag(fmt::format("{}//{}//Total", cfg->niceName(), uniqueName()));

            // Determine stability of energy
            // Check number of points already stored for the Configuration
            double grad = 0.0;
            auto stable = false;
            if (stabilityWindow > totalEnergyArray.nValues())
                Messenger::print("Too few points to assess stability.\n");
            else
            {
                double yMean;
                grad = Regression::linear(totalEnergyArray, stabilityWindow, yMean);
                double thresholdValue = fabs(stabilityThreshold * yMean);
                stable = fabs(grad) < thresholdValue;

                Messenger::print("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
                                 "{:e}, stable = {}).\n",
                                 stabilityWindow, grad, thresholdValue, DissolveSys::btoa(stable));
            }

            // Set variable in Configuration
            GenericListHelper<double>::realise(cfg->moduleData(), "EnergyGradient", "", GenericItem::InRestartFileFlag) = grad;
            GenericListHelper<bool>::realise(cfg->moduleData(), "EnergyStable", "", GenericItem::InRestartFileFlag) = stable;
            GenericListHelper<Data1D>::realise(cfg->moduleData(), "EnergyStability", "", GenericItem::InRestartFileFlag)
                .addPoint(dissolve.iteration(), stable);

            // If writing to a file, append it here
            if (saveData)
            {
                LineParser parser;
                std::string filename = fmt::format("{}.energy.txt", cfg->niceName());

                if (!DissolveSys::fileExists(filename))
                {
                    parser.openOutput(filename);
                    parser.writeLineF("# Energies for Configuration '{}'.\n", cfg->name());
                    parser.writeLine("# All values in kJ/mol.\n");
                    parser.writeLine("# Iteration   Total         Inter         Bond          Angle        "
                                     " Torsion       Gradient      S?\n");
                }
                else
                    parser.appendOutput(filename);
                parser.writeLineF("  {:10d}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {}\n",
                                  dissolve.iteration(), interEnergy + intraEnergy, interEnergy, bondEnergy, angleEnergy,
                                  torsionEnergy, grad, stable);
                parser.closeFiles();
            }
        }

        Messenger::print("\n");
    }

    return true;
}
