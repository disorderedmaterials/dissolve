// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/energykernel.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "modules/energy/energy.h"

// Run set-up stage
bool EnergyModule::setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // For the Configuration target add a flag to its moduleData (which is *not* stored in the restart file) to specify that we
    // are targeting it
    if (targetConfiguration_)
        dissolve.processingModuleData().realise<bool>("IsEnergyModuleTarget", targetConfiguration_->niceName(),
                                                      GenericItem::ProtectedFlag) = true;

    return true;
}

// Run main processing
bool EnergyModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    /*
     * Calculate Energy for the target Configuration(s)
     *
     * This is a parallel routine, with processes operating in groups, unless in TEST mode.
     */

    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", uniqueName());

    auto strategy = procPool.bestStrategy();

    // Print parameter summary
    if (test_)
    {
        Messenger::print("Energy: All energies will be calculated in serial test mode and compared to "
                         "production values.\n");
        if (testAnalytic_)
            Messenger::print("Energy: Exact, analytical potential will be used in test.");
        if (testReferenceInter_)
            Messenger::print("Energy: Reference interatomic energy is {:15.9e} kJ/mol.\n", testReferenceInter_.value());
        if (testReferenceIntra_)
            Messenger::print("Energy: Reference intramolecular energy is {:15.9e} kJ/mol.\n", testReferenceIntra_.value());
    }

    Messenger::print("\n");

    Messenger::print("Calculating total energy for Configuration '{}'...\n", targetConfiguration_->name());
    // Calculate the total energy
    if (test_)
    {
        /*
         * Calculate the total energy of the system using a basic loop on each process, and then compare with
         * production routines.
         */

        /*
         * Test Calculation Begins
         */

        const PotentialMap &potentialMap = dissolve.potentialMap();
        auto correctInterEnergy = 0.0, correctIntraEnergy = 0.0, correctSelfEnergy = 0.0;

        double r, angle;
        Atom *i, *j;
        Vec3<double> vecji, vecjk, veckl;
        std::shared_ptr<Molecule> molN, molM;
        const auto *box = targetConfiguration_->box();
        double scale;
        const auto cutoff = dissolve.potentialMap().range();

        Timer testTimer;

        // Calculate interatomic energy in a loop over defined Molecules
        for (auto n = 0; n < targetConfiguration_->nMolecules(); ++n)
        {
            molN = targetConfiguration_->molecule(n);

            // Molecule self-energy
            for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
            {
                i = molN->atom(ii);

                for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
                {
                    j = molN->atom(jj);

                    // Get interatomic distance
                    r = box->minimumDistance(i->r(), j->r());
                    if (r > cutoff)
                        continue;

                    // Get intramolecular scaling of atom pair
                    scale = i->scaling(j);
                    if (scale < 1.0e-3)
                        continue;

                    if (testAnalytic_)
                        correctSelfEnergy += potentialMap.analyticEnergy(i, j, r) * scale;
                    else
                        correctSelfEnergy += potentialMap.energy(*i, *j, r) * scale;
                }
            }

            // Molecule-molecule energy
            for (auto m = n + 1; m < targetConfiguration_->nMolecules(); ++m)
            {
                molM = targetConfiguration_->molecule(m);

                // Double loop over atoms
                for (auto ii = 0; ii < molN->nAtoms(); ++ii)
                {
                    i = molN->atom(ii);

                    for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                    {
                        j = molM->atom(jj);

                        // Get interatomic distance and check cutoff
                        r = box->minimumDistance(i->r(), j->r());
                        if (r > cutoff)
                            continue;

                        if (testAnalytic_)
                            correctInterEnergy += potentialMap.analyticEnergy(i, j, r);
                        else
                            correctInterEnergy += potentialMap.energy(*i, *j, r);
                    }
                }
            }

            // Bond energy
            for (const auto &bond : molN->species()->bonds())
            {
                r = targetConfiguration_->box()->minimumDistance(molN->atom(bond.indexI())->r(),
                                                                 molN->atom(bond.indexJ())->r());
                correctIntraEnergy += bond.energy(r);
            }

            // Angle energy
            for (const auto &angle : molN->species()->angles())
            {
                // Get vectors 'j-i' and 'j-k'
                vecji = targetConfiguration_->box()->minimumVector(molN->atom(angle.indexJ())->r(),
                                                                   molN->atom(angle.indexI())->r());
                vecjk = targetConfiguration_->box()->minimumVector(molN->atom(angle.indexJ())->r(),
                                                                   molN->atom(angle.indexK())->r());

                // Calculate angle and determine angle energy
                vecji.normalise();
                vecjk.normalise();
                correctIntraEnergy += angle.energy(Box::angleInDegrees(vecji, vecjk));
            }

            // Torsion energy
            for (const auto &torsion : molN->species()->torsions())
            {
                // Get vectors 'j-i', 'j-k' and 'k-l'
                vecji = targetConfiguration_->box()->minimumVector(molN->atom(torsion.indexJ())->r(),
                                                                   molN->atom(torsion.indexI())->r());
                vecjk = targetConfiguration_->box()->minimumVector(molN->atom(torsion.indexJ())->r(),
                                                                   molN->atom(torsion.indexK())->r());
                veckl = targetConfiguration_->box()->minimumVector(molN->atom(torsion.indexK())->r(),
                                                                   molN->atom(torsion.indexL())->r());

                angle = Box::torsionInDegrees(vecji, vecjk, veckl);

                // Determine Torsion energy
                correctIntraEnergy += torsion.energy(angle);
            }

            // Improper energy
            for (const auto &imp : molN->species()->impropers())
            {
                // Get vectors 'j-i', 'j-k' and 'k-l'
                vecji =
                    targetConfiguration_->box()->minimumVector(molN->atom(imp.indexJ())->r(), molN->atom(imp.indexI())->r());
                vecjk =
                    targetConfiguration_->box()->minimumVector(molN->atom(imp.indexJ())->r(), molN->atom(imp.indexK())->r());
                veckl =
                    targetConfiguration_->box()->minimumVector(molN->atom(imp.indexK())->r(), molN->atom(imp.indexL())->r());

                angle = Box::torsionInDegrees(vecji, vecjk, veckl);

                // Determine improper energy
                correctIntraEnergy += imp.energy(angle);
            }
        }

        // Add the self energy into the total interatomic energy
        correctInterEnergy += correctSelfEnergy;

        testTimer.stop();

        Messenger::print("Correct interatomic pairpotential energy (total) is {:15.9e} kJ/mol\n", correctInterEnergy);
        Messenger::print("Correct interatomic pairpotential (within molecules) is {:15.9e} kJ/mol\n", correctSelfEnergy);
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
        auto interEnergy = interAtomicEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
        interTimer.stop();

        // Calculate intramolecular energy
        Timer intraTimer;
        auto intraEnergy = intraMolecularEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
        intraTimer.stop();

        // Calculate total interatomic energy from molecules
        Timer moleculeTimer;
        EnergyKernel energyKernel(procPool, targetConfiguration_->box(), targetConfiguration_->cells(), dissolve.potentialMap(),
                                  cutoff);
        auto molecularEnergy = 0.0;
        for (const auto &mol : targetConfiguration_->molecules())
            molecularEnergy += energyKernel.energy(*mol, ProcessPool::subDivisionStrategy(strategy));
        // In the typical case where there is more than one molecule, our sum will contain double the intermolecular
        // pairpotential energy, and zero intramolecular energy
        if (targetConfiguration_->nMolecules() > 1)
            molecularEnergy *= 0.5;
        molecularEnergy += correctSelfEnergy;
        moleculeTimer.stop();

        Messenger::print("Production interatomic pairpotential energy is {:15.9e} kJ/mol\n", interEnergy);
        Messenger::print("Production intramolecular energy is {:15.9e} kJ/mol\n", intraEnergy);
        Messenger::print("Total production energy is {:15.9e} kJ/mol\n", interEnergy + intraEnergy);
        Messenger::print("Molecular energy (excluding bound terms) is {:15.9e} kJ/mol\n", molecularEnergy);
        Messenger::print("Time to do interatomic energy was {}.\n", interTimer.totalTimeString());
        Messenger::print("Time to do intramolecular energy was {}.\n", intraTimer.totalTimeString());
        Messenger::print("Time to do intermolecular energy was {}.\n", moleculeTimer.totalTimeString());

        /*
         * Production Calculation Ends
         */

        // Compare production vs reference values
        double delta;
        if (testReferenceInter_)
        {
            delta = testReferenceInter_.value() - correctInterEnergy;
            Messenger::print("Reference interatomic energy delta with correct value is {:15.9e} kJ/mol and "
                             "is {} (threshold is {:10.3e} kJ/mol)\n",
                             delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
            if (!procPool.allTrue(fabs(delta) < testThreshold_))
                return false;

            delta = testReferenceInter_.value() - interEnergy;
            Messenger::print("Reference interatomic energy delta with production value is {:15.9e} kJ/mol "
                             "and is {} (threshold is {:10.3e} kJ/mol)\n",
                             delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
            if (!procPool.allTrue(fabs(delta) < testThreshold_))
                return false;
        }
        if (testReferenceIntra_)
        {
            delta = testReferenceIntra_.value() - correctIntraEnergy;
            Messenger::print("Reference intramolecular energy delta with correct value is {:15.9e} kJ/mol "
                             "and is {} (threshold is {:10.3e} kJ/mol)\n",
                             delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
            if (!procPool.allTrue(fabs(delta) < testThreshold_))
                return false;

            delta = testReferenceIntra_.value() - intraEnergy;
            Messenger::print("Reference intramolecular energy delta with production value is {:15.9e} kJ/mol "
                             "and is {} (threshold is {:10.3e} kJ/mol)\n",
                             delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
            if (!procPool.allTrue(fabs(delta) < testThreshold_))
                return false;
        }

        // Compare production vs 'correct' values
        auto interDelta = correctInterEnergy - interEnergy;
        auto intraDelta = correctIntraEnergy - intraEnergy;
        auto moleculeDelta = correctInterEnergy - molecularEnergy;
        Messenger::print("Comparing 'correct' with production values...\n");
        Messenger::print("Interatomic energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n", interDelta,
                         fabs(interDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        Messenger::print("Intramolecular energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                         intraDelta, fabs(intraDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        Messenger::print("Intermolecular energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                         moleculeDelta, fabs(moleculeDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);

        // All OK?
        if (!procPool.allTrue((fabs(interDelta) < testThreshold_) && (fabs(intraDelta) < testThreshold_) &&
                              (fabs(moleculeDelta) < testThreshold_)))
            return false;
    }
    else
    {
        /*
         * Calculates the total energy of the entire system.
         *
         * This is a serial routine (subroutines called from within are parallel).
         */

        // Calculate intermolecular energy
        Timer interTimer;
        auto interEnergy = interAtomicEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
        interTimer.stop();

        // Calculate intramolecular and intermolecular correction energy
        Timer intraTimer;
        double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;
        auto intraEnergy = intraMolecularEnergy(procPool, targetConfiguration_, dissolve.potentialMap(), bondEnergy,
                                                angleEnergy, torsionEnergy, improperEnergy);
        intraTimer.stop();

        Messenger::print("Time to do interatomic energy was {}, intramolecular energy was {}.\n", interTimer.totalTimeString(),
                         intraTimer.totalTimeString());
        Messenger::print("Total Energy (World) is {:15.9e} kJ/mol ({:15.9e} kJ/mol interatomic + {:15.9e} kJ/mol "
                         "intramolecular).\n",
                         interEnergy + intraEnergy, interEnergy, intraEnergy);
        Messenger::print("Intramolecular contributions are - bonds = {:15.9e} kJ/mol, angles = {:15.9e} kJ/mol, "
                         "torsions = {:15.9e} kJ/mol, impropers = {:15.9e} kJ/mol.\n",
                         bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

        // Store current energies in the Configuration in case somebody else needs them
        auto &interData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Inter", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        interData.addPoint(dissolve.iteration(), interEnergy);
        auto &intraData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Intra", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        intraData.addPoint(dissolve.iteration(), intraEnergy);
        auto &bondData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Bond", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        bondData.addPoint(dissolve.iteration(), bondEnergy);
        auto &angleData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Angle", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        angleData.addPoint(dissolve.iteration(), angleEnergy);
        auto &torsionData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Torsions", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        torsionData.addPoint(dissolve.iteration(), torsionEnergy);
        auto &improperData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Impropers", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        improperData.addPoint(dissolve.iteration(), improperEnergy);

        // Append to arrays of total energies
        auto &totalEnergyArray = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("{}//Total", targetConfiguration_->niceName()), uniqueName(), GenericItem::InRestartFileFlag);
        totalEnergyArray.addPoint(dissolve.iteration(), interEnergy + intraEnergy);

        // Determine stability of energy
        // Check number of points already stored for the Configuration
        auto grad = 0.0;
        auto stable = false;
        if (stabilityWindow_ > totalEnergyArray.nValues())
            Messenger::print("Too few points to assess stability.\n");
        else
        {
            auto yMean = 0.0;
            grad = Regression::linear(totalEnergyArray, stabilityWindow_, yMean);
            auto thresholdValue = fabs(stabilityThreshold_ * yMean);
            stable = fabs(grad) < thresholdValue;

            Messenger::print("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
                             "{:e}, stable = {}).\n",
                             stabilityWindow_, grad, thresholdValue, DissolveSys::btoa(stable));
        }

        // Set energy data under the configuration's prefix
        dissolve.processingModuleData().realise<double>("EnergyGradient", targetConfiguration_->niceName(),
                                                        GenericItem::InRestartFileFlag) = grad;
        dissolve.processingModuleData().realise<bool>("EnergyStable", targetConfiguration_->niceName(),
                                                      GenericItem::InRestartFileFlag) = stable;

        // If writing to a file, append it here
        if (save_)
        {
            LineParser parser;
            std::string filename = fmt::format("{}.energy.txt", targetConfiguration_->niceName());

            if (!DissolveSys::fileExists(filename))
            {
                parser.openOutput(filename);
                parser.writeLineF("# Energies for Configuration '{}'.\n", targetConfiguration_->name());
                parser.writeLine("# All values in kJ/mol.\n");
                parser.writeLine("# Iteration   Total         Inter         Bond          Angle        "
                                 " Torsion      Improper    Gradient      S?\n");
            }
            else
                parser.appendOutput(filename);
            parser.writeLineF("  {:10d}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {}\n",
                              dissolve.iteration(), interEnergy + intraEnergy, interEnergy, bondEnergy, angleEnergy,
                              torsionEnergy, improperEnergy, grad, stable);
            parser.closeFiles();
        }
    }

    Messenger::print("\n");

    return true;
}
