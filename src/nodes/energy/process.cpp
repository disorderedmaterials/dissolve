// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "nodes/dissolve.h"
#include "nodes/energy/energy.h"

// Run main processing
NodeConstants::ProcessResult EnergyNode::process()
{
    message("\n");

    message("Calculating total energy for Configuration '{}'...\n", targetConfiguration_->name());

    /*
     * Calculates the total energy of the entire system.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    auto kernel = dissolveGraph()->createEnergyKernel(targetConfiguration_);
    auto &potentialMap = kernel->potentialMap();

    // Calculate pair potential energy
    Timer interTimer;
    auto ppEnergy = pairPotentialEnergy(targetConfiguration_, potentialMap);
    interTimer.stop();

    // Calculate intra-molecular (bound) energy
    Timer intraTimer;
    double bondEnergy, angleEnergy, torsionEnergy, improperEnergy;
    auto boundEnergy =
        intraMolecularEnergy(targetConfiguration_, potentialMap, bondEnergy, angleEnergy, torsionEnergy, improperEnergy);
    intraTimer.stop();

    message("Time to do interatomic energy was {}, intramolecular energy was {}.\n", interTimer.totalTimeString(),
            intraTimer.totalTimeString());
    message("Total Energy (World) is {:15.9e} kJ/mol ({:15.9e} kJ/mol pair potential + {:15.9e} kJ/mol "
            "intramolecular).\n",
            ppEnergy.total() + boundEnergy, ppEnergy.total(), boundEnergy);
    message("Intramolecular contributions are - bonds = {:15.9e} kJ/mol, angles = {:15.9e} kJ/mol, "
            "torsions = {:15.9e} kJ/mol, impropers = {:15.9e} kJ/mol.\n",
            bondEnergy, angleEnergy, torsionEnergy, improperEnergy);

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
        grad = Regression::linearGradient(totalEnergyArray, stabilityWindow_, yMean);
        auto thresholdValue = fabs(stabilityThreshold_ * yMean);
        stable = fabs(grad) < thresholdValue;

        message("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
                "{:e}, stable = {}).\n",
                stabilityWindow_, grad, thresholdValue, DissolveSys::btoa(stable));
    }

    targetConfiguration_->setEnergyGradient(grad);
    targetConfiguration_->setEnergyStable(stable);

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
                          dissolve().iteration(), ppEnergy.total() + boundEnergy, ppEnergy.total(), ppEnergy.interMolecular,
                          bondEnergy, angleEnergy, torsionEnergy, improperEnergy, grad, stable);
        parser.closeFiles();
    }
    return NodeConstants::ProcessResult::Success;
}
