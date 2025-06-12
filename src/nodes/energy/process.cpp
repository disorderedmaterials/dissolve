// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "module/context.h"
#include "nodes/energy/energy.h"

// Run set-up stage
bool EnergyModule::setUp(Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // For the Configuration target add a flag to its moduleData (which is *not* stored in the restart file) to specify that we
    // are targeting it
    if (targetConfiguration_)
    {
        /*
        dissolve().processingModuleData().realise<bool>("IsEnergyModuleTarget", targetConfiguration_->niceName(),
            GenericItem::ProtectedFlag) = true;
        */

    }
    return true;
}

// Run main processing
NodeConstants::ProcessResult EnergyModule::process()
{
    // Print parameter summary
    if (test_)
        message("Energy: Production energies will be tested against analytical equivalents.\n");

    message("\n");

    message("Calculating total energy for Configuration '{}'...\n", targetConfiguration_->name());

    /*
     * Calculates the total energy of the entire system.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

     // Calculate pair potential energy
    Timer interTimer;
    auto ppEnergy =
        pairPotentialEnergy(processPool(), targetConfiguration_, dissolve().potentialMap());
    interTimer.stop();

    // Calculate intra-molecular (bound) energy
    Timer intraTimer;
    double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;
    auto boundEnergy =
        intraMolecularEnergy(processPool(), targetConfiguration_, dissolve().potentialMap(),
            bondEnergy, angleEnergy, torsionEnergy, improperEnergy);
    intraTimer.stop();

    message("Time to do interatomic energy was {}, intramolecular energy was {}.\n", interTimer.totalTimeString(),
        intraTimer.totalTimeString());
    message("Total Energy (World) is {:15.9e} kJ/mol ({:15.9e} kJ/mol pair potential + {:15.9e} kJ/mol "
        "intramolecular).\n",
        ppEnergy.total() + boundEnergy, ppEnergy.total(), boundEnergy);
    message("Intramolecular contributions are - bonds = {:15.9e} kJ/mol, angles = {:15.9e} kJ/mol, "
        "torsions = {:15.9e} kJ/mol, impropers = {:15.9e} kJ/mol.\n",
        bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

    // Store current energies in the Configuration in case somebody else needs them
    /*
    auto& interData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//PairPotential", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D interData;
    interData.addPoint(dissolve().iteration(), ppEnergy.total());
    /*
    auto& intraData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Bound", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D intraData;
    intraData.addPoint(dissolve().iteration(), boundEnergy);
    /*
    auto& bondData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Bond", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D bondData;
    bondData.addPoint(dissolve().iteration(), bondEnergy);
    /*
    auto& angleData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Angle", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D angleData;
    angleData.addPoint(dissolve().iteration(), angleEnergy);
    /*
    auto& torsionData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Torsion", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D torsionData;
    torsionData.addPoint(dissolve().iteration(), torsionEnergy);
    /*
    auto& improperData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Improper", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D improperData;
    improperData.addPoint(dissolve().iteration(), improperEnergy);
    /*
    auto& cohesiveData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Cohesive", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D cohesiveData;
    cohesiveData.addPoint(dissolve().iteration(), ppEnergy.interMolecular());
    /*
    auto& intraPPData = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//IntraPP", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D intraPPData;
    intraPPData.addPoint(dissolve().iteration(), ppEnergy.intraMolecular());

    // Append to arrays of total energies
    /*
    auto& totalEnergyArray = dissolve().processingModuleData().realise<Data1D>(
        std::format("{}//Total", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    */
    Data1D totalEnergyArray;
    totalEnergyArray.addPoint(dissolve().iteration(), ppEnergy.total() + boundEnergy);

    // Determine stability of energy
    // Check number of points already stored for the Configuration
    auto grad = 0.0;
    auto stable = false;
    if (stabilityWindow_ > totalEnergyArray.nValues())
        message("Too few points to assess stability.\n");
    else
    {
        auto yMean = 0.0;
        grad = Regression::linear(totalEnergyArray, stabilityWindow_, yMean);
        auto thresholdValue = fabs(stabilityThreshold_ * yMean);
        stable = fabs(grad) < thresholdValue;

        message("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
            "{:e}, stable = {}).\n",
            stabilityWindow_, grad, thresholdValue, DissolveSys::btoa(stable));
    }

    // Set energy data under the configuration's prefix
    /*
    dissolve().processingModuleData().realise<double>("EnergyGradient", targetConfiguration_->niceName(),
        GenericItem::InRestartFileFlag) = grad;
    dissolve().processingModuleData().realise<bool>("EnergyStable", targetConfiguration_->niceName(),
        GenericItem::InRestartFileFlag) = stable;
    */

    // If writing to a file, append it here
    if (save_)
    {
        LineParser parser;
        std::string filename = std::format("{}.energy.txt", targetConfiguration_->niceName());

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
            dissolve().iteration(), ppEnergy.total() + boundEnergy, ppEnergy.total(),
            ppEnergy.interMolecular(), bondEnergy, angleEnergy, torsionEnergy, improperEnergy, grad, stable);
        parser.closeFiles();
    }

    // Test against analytic energy?
    if (test_)
    {
        /*
         * Calculate the total energy of the system using a basic loop on each process, and then compare with
         * production routines.
         */

        const PotentialMap& potentialMap = dissolve().potentialMap();
        auto correctInterEnergy = 0.0, correctIntraEnergy = 0.0, correctSelfEnergy = 0.0;

        double r, angle;
        Atom* i, * j;
        Vector3 vecji, vecjk, veckl;
        std::shared_ptr<Molecule> molN, molM;
        const auto* box = targetConfiguration_->box();
        const auto cutoff = dissolve().potentialMap().range();

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
                    auto&& [scalingType, elec14, vdw14] = i->scaling(j);
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
            for (const auto& bond : molN->species()->bonds())
            {
                r = targetConfiguration_->box()->minimumDistance(molN->atom(bond.indexI())->r(),
                    molN->atom(bond.indexJ())->r());
                correctIntraEnergy += bond.energy(r);
            }

            // Angle energy
            for (const auto& angle : molN->species()->angles())
            {
                correctIntraEnergy += angle.energy(targetConfiguration_->box()->angleInRadians(
                    molN->atom(angle.indexI())->r(), molN->atom(angle.indexJ())->r(), molN->atom(angle.indexK())->r()));
            }

            // Torsion energy
            for (const auto& torsion : molN->species()->torsions())
            {
                correctIntraEnergy += torsion.energy(targetConfiguration_->box()->torsionInRadians(
                    molN->atom(torsion.indexI())->r(), molN->atom(torsion.indexJ())->r(), molN->atom(torsion.indexK())->r(),
                    molN->atom(torsion.indexL())->r()));
            }

            // Improper energy
            for (const auto& imp : molN->species()->impropers())
            {
                correctIntraEnergy += imp.energy(targetConfiguration_->box()->torsionInRadians(
                    molN->atom(imp.indexI())->r(), molN->atom(imp.indexJ())->r(), molN->atom(imp.indexK())->r(),
                    molN->atom(imp.indexL())->r()));
            }
        }

        // Add the self energy into the total interatomic energy
        correctInterEnergy += correctSelfEnergy;

        testTimer.stop();

        message("Correct interatomic pairpotential energy (total) is {:15.9e} kJ/mol\n", correctInterEnergy);
        message("Correct interatomic pairpotential (within molecules) is {:15.9e} kJ/mol\n", correctSelfEnergy);
        message("Correct intramolecular energy is {:15.9e} kJ/mol\n", correctIntraEnergy);
        message("Correct total energy is {:15.9e} kJ/mol\n", correctInterEnergy + correctIntraEnergy);
        message("Time to do total (test) energy was {}.\n", testTimer.totalTimeString());

        // Calculate total interatomic energy from molecules
        Timer moleculeTimer;
        auto kernel = KernelProducer::energyKernel(targetConfiguration_, processPool(),
            dissolve().potentialMap(), cutoff);
        auto molecularPPEnergyInter = kernel->totalMoleculePairPotentialEnergy(false).total();
        auto molecularPPEnergyFull = kernel->totalMoleculePairPotentialEnergy(true).total();
        moleculeTimer.stop();

        message("Production interatomic pairpotential energy is {:15.9e} kJ/mol\n", ppEnergy.total());
        message("Production intramolecular energy is {:15.9e} kJ/mol\n", boundEnergy);
        message("Total production energy is {:15.9e} kJ/mol\n", ppEnergy.total() + boundEnergy);
        message("Molecular pairpotential energy (excluding intramolecular terms) is {:15.9e} kJ/mol\n",
            molecularPPEnergyInter);
        message("Molecular pairpotential energy (full) is {:15.9e} kJ/mol\n", molecularPPEnergyFull);
        message("Time to do interatomic energy was {}.\n", interTimer.totalTimeString());
        message("Time to do intramolecular energy was {}.\n", intraTimer.totalTimeString());
        message("Time to do intermolecular energy was {}.\n", moleculeTimer.totalTimeString());

        // Compare production vs 'correct' values
        auto interDelta = correctInterEnergy - ppEnergy.total();
        auto intraDelta = correctIntraEnergy - boundEnergy;
        auto moleculeDeltaA = correctInterEnergy - molecularPPEnergyFull;
        auto moleculeDeltaB = correctInterEnergy - (molecularPPEnergyInter + correctSelfEnergy);
        message("Comparing 'correct' with production values...\n");
        message("Interatomic energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n", interDelta,
            fabs(interDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        message("Intramolecular energy delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
            intraDelta, fabs(intraDelta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        message("Molecular pairpotential energy A delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
            moleculeDeltaA, fabs(moleculeDeltaA) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        message("Molecular pairpotential energy B delta is {:15.9e} kJ/mol and is {} (threshold is {:10.3e} kJ/mol)\n",
            moleculeDeltaB, fabs(moleculeDeltaB) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);

        // All OK?
        if (!processPool().allTrue((fabs(interDelta) < testThreshold_) && (fabs(intraDelta) < testThreshold_) &&
            (fabs(moleculeDeltaA) < testThreshold_) &&
            (fabs(moleculeDeltaB) < testThreshold_)))
            return NodeConstants::ProcessResult::Failed;
    }

    message("\n");

    return NodeConstants::ProcessResult::Success;
}
