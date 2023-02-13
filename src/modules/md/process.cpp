// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/randombuffer.h"
#include "base/timer.h"
#include "data/atomicmasses.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/md/md.h"

// Run main processing
bool MDModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Get control parameters
    const auto maxForce = capForcesAt_ * 100.0; // To convert from kJ/mol to 10 J/mol
    auto rCut = cutoffDistance_.value_or(dissolve.pairPotentialRange());

    // Units
    // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
    // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
    const auto kb = 0.8314462;

    // Print argument/parameter summary
    Messenger::print("MD: Cutoff distance is {}\n", rCut);
    Messenger::print("MD: Number of steps = {}\n", nSteps_);
    Messenger::print("MD: Timestep type is '{}'\n", timestepType().keyword(timestepType_));
    if (onlyWhenEnergyStable_)
        Messenger::print("MD: Only perform MD if target Configuration energies are stable.\n");
    if (trajectoryFrequency_.value_or(0) > 0)
        Messenger::print("MD: Trajectory file will be appended every {} step(s).\n", trajectoryFrequency_.value());
    else
        Messenger::print("MD: Trajectory file off.\n");
    if (capForces_)
        Messenger::print("MD: Forces will be capped to {:10.3e} kJ/mol per atom per axis.\n", maxForce / 100.0);
    if (energyFrequency_.value_or(0) > 0)
        Messenger::print("MD: Energy will be calculated every {} step(s).\n", energyFrequency_.value());
    else
        Messenger::print("MD: Energy will be not be calculated.\n");
    if (outputFrequency_.value_or(0) > 0)
        Messenger::print("MD: Summary will be written every {} step(s).\n", outputFrequency_.value());
    else
        Messenger::print("MD: Summary will not be written.\n");
    if (!restrictToSpecies_.empty())
        Messenger::print("MD: Calculation will be restricted to species:\n",
                         joinStrings(restrictToSpecies_, "  ", [](const auto &sp) { return sp->name(); }));
    Messenger::print("\n");

    if (onlyWhenEnergyStable_)
    {
        auto stabilityResult = EnergyModule::checkStability(dissolve.processingModuleData(), targetConfiguration_);
        if (stabilityResult == EnergyModule::NotAssessable)
            return false;
        else if (stabilityResult == EnergyModule::EnergyUnstable)
        {
            Messenger::print("Skipping MD for Configuration '{}'.\n", targetConfiguration_->niceName());
            return true;
        }
    }

    // Get temperature from Configuration
    const auto temperature = targetConfiguration_->temperature();

    // Create arrays
    std::vector<double> mass(targetConfiguration_->nAtoms(), 0.0);
    std::vector<Vec3<double>> fIntra(targetConfiguration_->nAtoms()), fInter(targetConfiguration_->nAtoms()),
        accelerations(targetConfiguration_->nAtoms());

    // Variables
    auto nCapped = 0;
    auto &atoms = targetConfiguration_->atoms();
    double tInstant, ke, tScale, peInter, peIntra;

    // Determine target molecules from the restrictedSpecies vector (if any)
    std::vector<const Molecule *> targetMolecules;
    std::vector<int> free(targetConfiguration_->nAtoms(), 0);
    if (restrictToSpecies_.empty())
        std::fill(free.begin(), free.end(), 1);
    else
        for (const auto &mol : targetConfiguration_->molecules())
            if (std::find(restrictToSpecies_.begin(), restrictToSpecies_.end(), mol->species()) != restrictToSpecies_.end())
            {
                targetMolecules.push_back(mol.get());
                for (const auto &i : mol->atoms())
                    free[i->arrayIndex()] = 1;
            }

    /*
     * Calculation Begins
     */

    // Initialise the random number buffer for all processes
    RandomBuffer randomBuffer(procPool, ProcessPool::PoolProcessesCommunicator);

    // Read in or assign random velocities
    auto [velocities, status] = dissolve.processingModuleData().realiseIf<std::vector<Vec3<double>>>(
        fmt::format("{}//Velocities", targetConfiguration_->niceName()), name(), GenericItem::InRestartFileFlag);
    if ((status == GenericItem::ItemStatus::Created || randomVelocities_ ||
         velocities.size() != targetConfiguration_->nAtoms()) &&
        !intramolecularForcesOnly_)
    {
        // Show warning message on array size mismatch
        if (status != GenericItem::ItemStatus::Created && velocities.size() != targetConfiguration_->nAtoms())
            Messenger::warn(
                "Size of existing velocities array doesn't match the current configuration size - they will be ignored.");

        Messenger::print("Random initial velocities will be assigned.\n");
        velocities.resize(targetConfiguration_->nAtoms(), Vec3<double>());
        for (auto &&[v, iFree] : zip(velocities, free))
        {
            if (iFree)
                v.set(exp(randomBuffer.random() - 0.5), exp(randomBuffer.random() - 0.5), exp(randomBuffer.random() - 0.5));
            else
                v.zero();
            v /= sqrt(TWOPI);
        }
    }
    else if (intramolecularForcesOnly_)
    {
        Messenger::print("Only intramolecular forces will be calculated, so velocities will be zeroes.\n");
        velocities.resize(targetConfiguration_->nAtoms(), Vec3<double>());
        std::fill(velocities.begin(), velocities.end(), Vec3<double>());
    }
    else
        Messenger::print("Existing velocities will be used.\n");
    Messenger::print("\n");

    // Store atomic masses for future use
    for (auto &&[i, m] : zip(atoms, mass))
        m = AtomicMass::mass(i.speciesAtom()->Z());

    // Calculate total velocity and mass over all atoms
    Vec3<double> vCom;
    auto massSum = 0.0;
    for (auto &&[v, m, iFree] : zip(velocities, mass, free))
    {
        if (!iFree)
            continue;
        vCom += v * m;
        massSum += m;
    }

    // Finalise initial velocities (unless considering intramolecular forces only)
    if (!intramolecularForcesOnly_)
    {
        // Remove any velocity shift, and re-zero velocities on fixed atoms
        vCom /= massSum;
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [vCom](auto vel) { return vel - vCom; });
        for (auto &&[v, iFree] : zip(velocities, free))
            if (!iFree)
                v.zero();

        // Calculate instantaneous temperature
        ke = 0.0;
        for (auto &&[m, v] : zip(mass, velocities))
            ke += 0.5 * m * v.dp(v);
        tInstant = ke * 2.0 / (3.0 * atoms.size() * kb);

        // Rescale velocities for desired temperature
        tScale = sqrt(temperature / tInstant);
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto v) { return v * tScale; });
    }

    // Open trajectory file (if requested)
    LineParser trajParser;
    if (trajectoryFrequency_.value_or(0) > 0)
    {
        std::string trajectoryFile = fmt::format("{}.md.xyz", targetConfiguration_->name());
        if (procPool.isMaster())
        {
            if ((!trajParser.appendOutput(trajectoryFile)) || (!trajParser.isFileGoodForWriting()))
            {
                Messenger::error("Failed to open MD trajectory output file '{}'.\n", trajectoryFile);
                procPool.decideFalse();
                return false;
            }
            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return false;
    }

    // Write header
    if (outputFrequency_.value_or(0) > 0)
    {
        Messenger::print("                                             Energies (kJ/mol)\n");
        Messenger::print("  Step             T(K)         Kinetic      Inter        Intra        Total      "
                         "deltaT(ps)\n");
    }

    // Start a timer
    Timer timer, commsTimer(false);

    // If we're not using a fixed timestep the forces need to be available immediately
    if (timestepType_ != TimestepType::Fixed)
    {
        // Zero force arrays
        std::fill(fInter.begin(), fInter.end(), Vec3<double>());
        std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());

        if (targetMolecules.empty())
            intramolecularForcesOnly_ ? ForcesModule::internalMoleculeForces(procPool, targetConfiguration_,
                                                                             dissolve.potentialMap(), true, fInter, fIntra)
                                      : ForcesModule::totalForces(procPool, targetConfiguration_, dissolve.potentialMap(),
                                                                  fInter, fIntra, commsTimer);
        else
            intramolecularForcesOnly_
                ? ForcesModule::internalMoleculeForces(procPool, targetConfiguration_, dissolve.potentialMap(), true, fInter,
                                                       fIntra, targetMolecules)
                : ForcesModule::totalForces(procPool, targetConfiguration_, targetMolecules, dissolve.potentialMap(), fInter,
                                            fIntra, commsTimer);

        // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto f) { return f * 100.0; });
        std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto f) { return f * 100.0; });

        // If the strategy is Auto100 do a quick check on the timestep now
        if (timestepType_ == TimestepType::Automatic && !determineTimeStep(fInter, fIntra))
        {
            Messenger::print("Forces are currently too high for MD to proceed. Skipping this run.\n");
            return true;
        }
    }

    // Ready to do MD propagation of system
    auto step = 1;
    for (step = 1; step <= nSteps_; ++step)
    {
        // Get timestep
        auto optDT = determineTimeStep(fInter, fIntra);
        if (!optDT)
        {
            Messenger::warn("A reasonable timestep could not be determined. Stopping evolution.\n");
            break;
        }
        auto dT = *optDT;
        auto deltaTSq = dT * dT;

        // Velocity Verlet first stage (A)
        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
        // B:  a(t+dt) = F(t+dt)/m
        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
        for (auto &&[i, v, a] : zip(atoms, velocities, accelerations))
        {
            // Propagate positions (by whole step)...
            i.translateCoordinates(v * dT + a * 0.5 * deltaTSq);

            // ...velocities (by half step)...
            v += a * 0.5 * dT;
        }

        // Update Cell contents / Atom locations
        targetConfiguration_->updateCellContents();

        // Zero force arrays
        std::fill(fInter.begin(), fInter.end(), Vec3<double>());
        std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());

        // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        if (targetMolecules.empty())
            intramolecularForcesOnly_
                ? ForcesModule::internalMoleculeForces(procPool, targetConfiguration_, dissolve.potentialMap(), true, fInter,
                                                       fIntra)
                : ForcesModule::totalForces(procPool, targetConfiguration_, dissolve.potentialMap(), fInter, fIntra);
        else
            intramolecularForcesOnly_
                ? ForcesModule::internalMoleculeForces(procPool, targetConfiguration_, dissolve.potentialMap(), true, fInter,
                                                       fIntra, targetMolecules)
                : ForcesModule::totalForces(procPool, targetConfiguration_, targetMolecules, dissolve.potentialMap(), fInter,
                                            fIntra);
        std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto f) { return f * 100.0; });
        std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto f) { return f * 100.0; });

        // Cap forces
        if (capForces_)
            nCapped = capForces(maxForce, fInter, fIntra);

        // Velocity Verlet second stage (B) and velocity scaling
        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
        // B:  a(t+dt) = F(t+dt)/m
        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
        ke = 0.0;
        for (auto &&[f1, f2, v, a, m] : zip(fInter, fIntra, velocities, accelerations, mass))
        {
            // Determine new accelerations
            a = (f1 + f2) / m;

            // ..and finally velocities again (by second half-step)
            v += a * 0.5 * dT;

            ke += 0.5 * m * v.dp(v);
        }

        // Rescale velocities for desired temperature
        tInstant = ke * 2.0 / (3.0 * targetConfiguration_->nAtoms() * kb);
        tScale = sqrt(temperature / tInstant);
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto &v) { return v * tScale; });

        // Convert ke from 10J/mol to kJ/mol
        ke *= 0.01;

        // Write step summary?
        if ((step == 1) || (step % outputFrequency_.value_or(0) == 0))
        {
            // Include total energy term?
            if ((energyFrequency_.value_or(0) > 0) && (step % energyFrequency_.value_or(0) == 0))
            {
                peInter = EnergyModule::interAtomicEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
                peIntra = EnergyModule::intraMolecularEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}\n", step,
                                 tInstant, ke, peInter, peIntra, ke + peIntra + peInter, dT);
            }
            else
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}                                          {:10.3e}\n", step,
                                 tInstant, ke, dT);
        }

        // Save trajectory frame
        if (trajectoryFrequency_.value_or(0) > 0 && (step % trajectoryFrequency_.value_or(0) == 0))
        {
            if (procPool.isMaster())
            {
                // Write number of atoms
                trajParser.writeLineF("{}\n", targetConfiguration_->nAtoms());

                // Construct and write header
                std::string header = fmt::format("Step {} of {}, T = {:10.3e}, ke = {:10.3e}", step, nSteps_, tInstant, ke);
                if ((energyFrequency_.value_or(0) > 0) && (step % energyFrequency_.value_or(0) == 0))
                    header += fmt::format(", inter = {:10.3e}, intra = {:10.3e}, tot = {:10.3e}", peInter, peIntra,
                                          ke + peInter + peIntra);
                if (!trajParser.writeLine(header))
                {
                    procPool.decideFalse();
                    return false;
                }

                // Write Atoms
                for (const auto &i : atoms)
                {
                    if (!trajParser.writeLineF("{:<3}   {:10.3f}  {:10.3f}  {:10.3f}\n", Elements::symbol(i.speciesAtom()->Z()),
                                               i.r().x, i.r().y, i.r().z))
                    {
                        procPool.decideFalse();
                        return false;
                    }
                }

                procPool.decideTrue();
            }
            else if (!procPool.decision())
                return false;
        }
    }
    timer.stop();

    // Close trajectory file
    if (trajectoryFrequency_.value_or(0) > 0 && procPool.isMaster())
        trajParser.closeFiles();

    if (capForces_)
        Messenger::print("A total of {} forces were capped over the course of the dynamics ({:9.3e} per step).\n", nCapped,
                         double(nCapped) / nSteps_);
    Messenger::print("{} steps performed ({} work, {} comms)\n", step - 1, timer.totalTimeString(),
                     commsTimer.totalTimeString());

    // Increment configuration changeCount
    if (step > 1)
        targetConfiguration_->incrementContentsVersion();

    /*
     * Calculation End
     */

    return true;
}
