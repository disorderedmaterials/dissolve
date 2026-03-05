// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "modules/energy/energy.h"

// Run set-up stage
bool EnergyModule::setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // For the Configuration target add a flag to its moduleData (which is *not* stored in the restart file) to specify that we
    // are targeting it
    if (targetConfiguration_)
        dissolve.processingModuleData().realise<bool>("IsEnergyModuleTarget", targetConfiguration_->name(),
                                                      GenericItem::ProtectedFlag) = true;

    return true;
}

// Run main processing
Module::ExecutionResult EnergyModule::process(Dissolve &dissolve)
{
    // Print parameter summary
    if (test_)
        Messenger::print("Energy: Production energies will be tested against analytical equivalents.\n");

    Messenger::print("\n");

    Messenger::print("Calculating total energy for Configuration '{}'...\n", targetConfiguration_->name());

    /*
     * Calculates the total energy of the entire system.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Calculate pair potential energy
    Timer interTimer;
    auto ppEnergy = pairPotentialEnergy(targetConfiguration_, dissolve.potentialMap());
    interTimer.stop();

    // Calculate intra-molecular (bound) energy
    Timer intraTimer;
    auto geomEnergy = geometryEnergy(targetConfiguration_, dissolve.potentialMap());
    intraTimer.stop();

    Messenger::print("Time to do interatomic energy was {}, intramolecular energy was {}.\n", interTimer.totalTimeString(),
                     intraTimer.totalTimeString());
    Messenger::print("Total Energy (World) is {:15.9e} kJ/mol ({:15.9e} kJ/mol pair potential + {:15.9e} kJ/mol "
                     "intramolecular).\n",
                     ppEnergy.total() + geomEnergy.total(), ppEnergy.total(), geomEnergy.total());
    Messenger::print("Intramolecular contributions are - bonds = {:15.9e} kJ/mol, angles = {:15.9e} kJ/mol, "
                     "torsions = {:15.9e} kJ/mol, impropers = {:15.9e} kJ/mol.\n",
                     geomEnergy.bondEnergy, geomEnergy.angleEnergy, geomEnergy.torsionEnergy, geomEnergy.improperEnergy);

    // Store current energies in the Configuration in case somebody else needs them
    auto &interData = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//PairPotential", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    interData.addPoint(dissolve.iteration(), ppEnergy.total());
    auto &intraData = dissolve.processingModuleData().realise<Data1D>(std::format("{}//Bound", targetConfiguration_->name()),
                                                                      name(), GenericItem::InRestartFileFlag);
    intraData.addPoint(dissolve.iteration(), geomEnergy.total());
    auto &bondData = dissolve.processingModuleData().realise<Data1D>(std::format("{}//Bond", targetConfiguration_->name()),
                                                                     name(), GenericItem::InRestartFileFlag);
    bondData.addPoint(dissolve.iteration(), geomEnergy.bondEnergy);
    auto &angleData = dissolve.processingModuleData().realise<Data1D>(std::format("{}//Angle", targetConfiguration_->name()),
                                                                      name(), GenericItem::InRestartFileFlag);
    angleData.addPoint(dissolve.iteration(), geomEnergy.angleEnergy);
    auto &torsionData = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//Torsion", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    torsionData.addPoint(dissolve.iteration(), geomEnergy.torsionEnergy);
    auto &improperData = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//Improper", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    improperData.addPoint(dissolve.iteration(), geomEnergy.improperEnergy);
    auto &cohesiveData = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//Cohesive", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    cohesiveData.addPoint(dissolve.iteration(), ppEnergy.interMolecular);
    auto &intraPPData = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//IntraPP", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    intraPPData.addPoint(dissolve.iteration(), ppEnergy.intraMolecular);

    // Append to arrays of total energies
    auto &totalEnergyArray = dissolve.processingModuleData().realise<Data1D>(
        std::format("{}//Total", targetConfiguration_->name()), name(), GenericItem::InRestartFileFlag);
    totalEnergyArray.addPoint(dissolve.iteration(), ppEnergy.total() + geomEnergy.total());

    // Determine stability of energy
    // Check number of points already stored for the Configuration
    auto grad = 0.0;
    auto stable = false;
    if (stabilityWindow_ > totalEnergyArray.nValues())
        Messenger::print("Too few points to assess stability.\n");
    else
    {
        auto yMean = 0.0;
        grad = Regression::linearGradient(totalEnergyArray, stabilityWindow_, yMean);
        auto thresholdValue = fabs(stabilityThreshold_ * yMean);
        stable = fabs(grad) < thresholdValue;

        Messenger::print("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
                         "{:e}, stable = {}).\n",
                         stabilityWindow_, grad, thresholdValue, DissolveSys::btoa(stable));
    }

    // Set energy data under the configuration's prefix
    dissolve.processingModuleData().realise<double>("EnergyGradient", targetConfiguration_->name(),
                                                    GenericItem::InRestartFileFlag) = grad;
    dissolve.processingModuleData().realise<bool>("EnergyStable", targetConfiguration_->name(),
                                                  GenericItem::InRestartFileFlag) = stable;

    // If writing to a file, append it here
    if (save_)
    {
        LineParser parser;
        std::string filename = std::format("{}.energy.txt", targetConfiguration_->name());

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
        parser.writeLineF("  {:10d}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {:12.6e}  {}\n",
                          dissolve.iteration(), ppEnergy.total() + geomEnergy.total(), ppEnergy.total(),
                          ppEnergy.interMolecular, geomEnergy.bondEnergy, geomEnergy.angleEnergy, geomEnergy.torsionEnergy,
                          geomEnergy.improperEnergy, grad, stable);
        parser.closeFiles();
    }

    // Test against analytic energy?
    if (test_)
    {
        /*
         * Calculate the total energy of the system using a basic loop on each process, and then compare with
         * production routines.
         */

        const PotentialMap &potentialMap = dissolve.potentialMap();
        auto correctInterEnergy = 0.0, correctIntraEnergy = 0.0, correctSelfEnergy = 0.0;

        double r;
        Atom *i, *j;
        std::shared_ptr<Molecule> molN, molM;
        const auto *box = targetConfiguration_->box();
        const auto cutoff = PairPotential::range();

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
                    auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        correctSelfEnergy += potentialMap.analyticEnergy(*i, *j, r);
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        correctSelfEnergy += potentialMap.analyticEnergy(*i, *j, r, elec14, vdw14);
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

                        correctInterEnergy += potentialMap.analyticEnergy(*i, *j, r);
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
                correctIntraEnergy += angle.energy(targetConfiguration_->box()->angleInRadians(
                    molN->atom(angle.indexI())->r(), molN->atom(angle.indexJ())->r(), molN->atom(angle.indexK())->r()));
            }

            // Torsion energy
            for (const auto &torsion : molN->species()->torsions())
            {
                correctIntraEnergy += torsion.energy(targetConfiguration_->box()->torsionInRadians(
                    molN->atom(torsion.indexI())->r(), molN->atom(torsion.indexJ())->r(), molN->atom(torsion.indexK())->r(),
                    molN->atom(torsion.indexL())->r()));
            }

            // Improper energy
            for (const auto &imp : molN->species()->impropers())
            {
                correctIntraEnergy += imp.energy(targetConfiguration_->box()->torsionInRadians(
                    molN->atom(imp.indexI())->r(), molN->atom(imp.indexJ())->r(), molN->atom(imp.indexK())->r(),
                    molN->atom(imp.indexL())->r()));
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

        // Calculate total interatomic energy from molecules
        Timer moleculeTimer;
        auto kernel = KernelProducer::energyKernel(targetConfiguration_, dissolve.potentialMap());
        auto [molecularPPEnergy, moleculesGeomEnergy] = kernel->totalEnergyMolecules();
        moleculeTimer.stop();

        Messenger::print("Production interatomic pairpotential energy is {:15.9e} kJ/mol\n", ppEnergy.total());
        Messenger::print("Production intramolecular energy is {:15.9e} kJ/mol\n", geomEnergy.total());
        Messenger::print("Total production energy is {:15.9e} kJ/mol\n", ppEnergy.total() + geomEnergy.total());
        Messenger::print("Molecular pairpotential energy (excluding intramolecular terms) is {:15.9e} kJ/mol\n",
                         molecularPPEnergyInter);
        Messenger::print("Molecular pairpotential energy (full) is {:15.9e} kJ/mol\n", molecularPPEnergyFull);
        Messenger::print("Time to do interatomic energy was {}.\n", interTimer.totalTimeString());
        Messenger::print("Time to do intramolecular energy was {}.\n", intraTimer.totalTimeString());
        Messenger::print("Time to do intermolecular energy was {}.\n", moleculeTimer.totalTimeString());

        // Compare production vs 'correct' values
        auto interDelta = correctInterEnergy - ppEnergy.total();
        auto intraDelta = correctIntraEnergy - geomEnergy.total();
        auto moleculeDeltaA = correctInterEnergy - molecularPPEnergyFull;
        auto moleculeDeltaB = correctInterEnergy - (molecularPPEnergyInter + correctSelfEnergy);
        Messenger::print("Comparing 'correct' with production values...\n");
        Messenger::print("Interatomic energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n", interDelta,
                         fabs(interDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        Messenger::print("Intramolecular energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                         intraDelta, fabs(intraDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        Messenger::print("Molecular pairpotential energy A delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                         moleculeDeltaA, fabs(moleculeDeltaA) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        Messenger::print("Molecular pairpotential energy B delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
                         moleculeDeltaB, fabs(moleculeDeltaB) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);

        // All OK?
        if (!((fabs(interDelta) < testThreshold_) && (fabs(intraDelta) < testThreshold_) &&
              (fabs(moleculeDeltaA) < testThreshold_) && (fabs(moleculeDeltaB) < testThreshold_)))
            return ExecutionResult::Failed;
    }

    Messenger::print("\n");

    return ExecutionResult::Success;
}
