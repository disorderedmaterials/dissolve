// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/randomBuffer.h"
#include "base/timer.h"
#include "data/atomicMasses.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/md/md.h"

// Run main processing
Module::ExecutionResult MDModule::process(ModuleContext &moduleContext)
{
    // Get control parameters
    const auto maxForce = capForcesAt_ * 100.0; // To convert from kJ/mol to 10 J/mol
    auto rCut = cutoffDistance_.value_or(moduleContext.dissolve().pairPotentialRange());

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
        Messenger::print("MD: Calculation will be restricted to species: {}\n",
                         joinStrings(restrictToSpecies_, "  ", [](const auto &sp) { return sp->name(); }));
    Messenger::print("\n");

    if (onlyWhenEnergyStable_)
    {
        auto stabilityResult =
            EnergyModule::checkStability(moduleContext.dissolve().processingModuleData(), targetConfiguration_);
        if (stabilityResult == EnergyModule::NotAssessable)
        {
            return ExecutionResult::Failed;
        }
        else if (stabilityResult == EnergyModule::EnergyUnstable)
        {
            Messenger::print("Skipping MD for Configuration '{}'.\n", targetConfiguration_->niceName());
            return ExecutionResult::NotExecuted;
        }
    }

    // Get temperature from Configuration
    const auto temperature = targetConfiguration_->temperature();

    // Create arrays
    std::vector<double> mass(targetConfiguration_->nAtoms(), 0.0);
    std::vector<Vec3<double>> fBound(targetConfiguration_->nAtoms()), fUnbound(targetConfiguration_->nAtoms()),
        accelerations(targetConfiguration_->nAtoms());

    // Variables
    auto nCapped = 0;
    auto &atoms = targetConfiguration_->atoms();
    double tInstant, ke, tScale, peBound;
    PairPotentialEnergyValue pePP;

    // Determine target molecules from the restrictedSpecies vector (if any)
    std::vector<const Molecule *> targetMolecules;
    std::vector<int> free(targetConfiguration_->nAtoms(), 0);
    if (restrictToSpecies_.empty())
    {
        std::fill(free.begin(), free.end(), 1);
    }
    else
        for (const auto &mol : targetConfiguration_->molecules())
            if (std::find(restrictToSpecies_.begin(), restrictToSpecies_.end(), mol->species()) != restrictToSpecies_.end())
            {
                targetMolecules.push_back(mol.get());
                auto offset = mol->globalAtomOffset();
                std::fill(free.begin() + offset, free.begin() + offset + mol->atoms().size(), 1);
            }

    /*
     * Calculation Begins
     */

    // Initialise the random number buffer for all processes
    RandomBuffer randomBuffer(moduleContext.processPool(), ProcessPool::PoolProcessesCommunicator);

    // Read in or assign random velocities
    auto [velocities, status] = moduleContext.dissolve().processingModuleData().realiseIf<std::vector<Vec3<double>>>(
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
    {
        Messenger::print("Existing velocities will be used.\n");
    }

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
        if (moduleContext.processPool().isMaster())
        {
            if ((!trajParser.appendOutput(trajectoryFile)) || (!trajParser.isFileGoodForWriting()))
            {
                Messenger::error("Failed to open MD trajectory output file '{}'.\n", trajectoryFile);
                moduleContext.processPool().decideFalse();
                return ExecutionResult::Failed;
            }
            moduleContext.processPool().decideTrue();
        }
        else if (!moduleContext.processPool().decision())
        {
            return ExecutionResult::Failed;
        }
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
        std::fill(fUnbound.begin(), fUnbound.end(), Vec3<double>());
        std::fill(fBound.begin(), fBound.end(), Vec3<double>());

        if (targetMolecules.empty())
            ForcesModule::totalForces(moduleContext.processPool(), targetConfiguration_,
                                      moduleContext.dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound, commsTimer);
        else
            ForcesModule::totalForces(moduleContext.processPool(), targetConfiguration_, targetMolecules,
                                      moduleContext.dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound, commsTimer);

        // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        std::transform(fUnbound.begin(), fUnbound.end(), fUnbound.begin(), [](auto f) { return f * 100.0; });
        std::transform(fBound.begin(), fBound.end(), fBound.begin(), [](auto f) { return f * 100.0; });

        // Check for suitable timestep
        if (!determineTimeStep(timestepType_, fixedTimestep_, fUnbound, fBound))
        {
            Messenger::print("Forces are currently too high for MD to proceed. Skipping this run.\n");
            return ExecutionResult::NotExecuted;
        }
    }

    // Ready to do MD propagation of system
    auto step = 1;
    for (step = 1; step <= nSteps_; ++step)
    {
        // Get timestep
        auto optDT = determineTimeStep(timestepType_, fixedTimestep_, fUnbound, fBound);
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

        // Update Atom locations
        targetConfiguration_->updateAtomLocations();

        // Zero force arrays
        std::fill(fUnbound.begin(), fUnbound.end(), Vec3<double>());
        std::fill(fBound.begin(), fBound.end(), Vec3<double>());

        // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        if (targetMolecules.empty())
            ForcesModule::totalForces(moduleContext.processPool(), targetConfiguration_,
                                      moduleContext.dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound, commsTimer);
        else
            ForcesModule::totalForces(moduleContext.processPool(), targetConfiguration_, targetMolecules,
                                      moduleContext.dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound, commsTimer);
        std::transform(fUnbound.begin(), fUnbound.end(), fUnbound.begin(), [](auto f) { return f * 100.0; });
        std::transform(fBound.begin(), fBound.end(), fBound.begin(), [](auto f) { return f * 100.0; });

        // Cap forces
        if (capForces_)
            nCapped = capForces(maxForce, fUnbound, fBound);

        // Velocity Verlet second stage (B) and velocity scaling
        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
        // B:  a(t+dt) = F(t+dt)/m
        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
        ke = 0.0;
        for (auto &&[f1, f2, v, a, m] : zip(fUnbound, fBound, velocities, accelerations, mass))
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
        if (outputFrequency_ && (step == 1 || (step % outputFrequency_.value() == 0)))
        {
            // Include total energy term?
            if (energyFrequency_ && (step % energyFrequency_.value() == 0))
            {
                pePP = EnergyModule::pairPotentialEnergy(moduleContext.processPool(), targetConfiguration_,
                                                         moduleContext.dissolve().potentialMap());
                peBound = EnergyModule::intraMolecularEnergy(moduleContext.processPool(), targetConfiguration_,
                                                             moduleContext.dissolve().potentialMap());
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}\n", step,
                                 tInstant, ke, pePP.total(), peBound, ke + peBound + pePP.total(), dT);
            }
            else
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}                                          {:10.3e}\n", step,
                                 tInstant, ke, dT);
        }

        // Save trajectory frame
        if (trajectoryFrequency_ && (step % trajectoryFrequency_.value() == 0))
        {
            if (moduleContext.processPool().isMaster())
            {
                // Write number of atoms
                trajParser.writeLineF("{}\n", targetConfiguration_->nAtoms());

                // Construct and write header
                std::string header = fmt::format("Step {} of {}, T = {:10.3e}, ke = {:10.3e}", step, nSteps_, tInstant, ke);
                if (energyFrequency_ && (step % energyFrequency_.value() == 0))
                    header += fmt::format(", inter = {:10.3e}, intra = {:10.3e}, tot = {:10.3e}", pePP.total(), peBound,
                                          ke + pePP.total() + peBound);
                if (!trajParser.writeLine(header))
                {
                    moduleContext.processPool().decideFalse();
                    return ExecutionResult::Failed;
                }

                // Write Atoms
                for (const auto &i : atoms)
                {
                    if (!trajParser.writeLineF("{:<3}   {:10.3f}  {:10.3f}  {:10.3f}\n", Elements::symbol(i.speciesAtom()->Z()),
                                               i.r().x, i.r().y, i.r().z))
                    {
                        moduleContext.processPool().decideFalse();
                        return ExecutionResult::Failed;
                    }
                }

                moduleContext.processPool().decideTrue();
            }
            else if (!moduleContext.processPool().decision())
            {
                return ExecutionResult::Failed;
            }
        }
    }
    timer.stop();

    // Close trajectory file
    if (trajectoryFrequency_.value_or(0) > 0 && moduleContext.processPool().isMaster())
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

    return ExecutionResult::Success;
}
