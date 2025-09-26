// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/mathFunc.h"
#include "nodes/dissolve.h"
#include "nodes/md/md.h"

// Run main processing
NodeConstants::ProcessResult MDNode::process()
{
    // Get numeric input data
    auto nSteps = nSteps_.asInteger();
    auto fixedTimestep = fixedTimestep_.asDouble();
    auto capForcesAt = capForcesAt_.asDouble();
    auto outputFrequency = outputFrequency_.value().asInteger();
    auto energyFrequency = energyFrequency_.value().asInteger();
    auto trajectoryFrequency = trajectoryFrequency_.value_or(Number(0)).asInteger();

    // Get control parameters
    const auto maxForce = capForcesAt * 100.0; // To convert from kJ/mol to 10 J/mol

    // Units
    // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
    // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
    const auto kb = 0.8314462;

    // Print argument/parameter summary
    message("Number of steps = {}\n", nSteps);
    message("Timestep type is '{}'\n", timestepType().keyword(timestepType_));
    if (onlyWhenEnergyStable_)
        message("Only perform MD if target Configuration energies are stable.\n");
    if (trajectoryFrequency > 0)
        message("Trajectory file will be appended every {} step(s).\n", trajectoryFrequency);
    else
        message("Trajectory file off.\n");
    if (capForces_)
        message("Forces will be capped to {:10.3e} kJ/mol per atom per axis.\n", maxForce / 100.0);
    if (energyFrequency > 0)
        message("Energy will be calculated every {} step(s).\n", energyFrequency);
    else
        message("Energy will be not be calculated.\n");
    if (outputFrequency > 0)
        message("Summary will be written every {} step(s).\n", outputFrequency);
    else
        message("Summary will not be written.\n");
    if (!restrictToSpecies_.empty())
        message("Calculation will be restricted to species: {}\n",
                joinStrings(restrictToSpecies_, "  ", [](const auto &sp) { return sp->name(); }));
    message("\n");

    auto kernel = dissolveGraph()->prepareEnergyCalculation(targetConfiguration_);

    /*
    if (onlyWhenEnergyStable_)
    {
        auto stabilityResult = EnergyModule::checkStability(targetConfiguration_);
        if (stabilityResult == EnergyModule::NotAssessable)
        {
            return NodeConstants::ProcessResult::Failed;
        }
        else if (stabilityResult == EnergyModule::EnergyUnstable)
        {
            Messenger::print("Skipping MD for Configuration '{}'.\n", targetConfiguration_->name());
            return NodeConstants::ProcessResult::Unchanged;
        }
    }
    */

    // Get temperature from Configuration
    const auto temperature = targetConfiguration_->temperature();

    // Create arrays
    std::vector<double> mass(targetConfiguration_->nAtoms(), 0.0);
    std::vector<Vector3> fBound(targetConfiguration_->nAtoms()), fUnbound(targetConfiguration_->nAtoms()),
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

    if (!velocities_)
        velocities_.emplace();
    auto &velocities = velocities_.value();
    auto status = GenericItem::ItemStatus::Created;
    if ((status == GenericItem::ItemStatus::Created || randomVelocities_ ||
         velocities.size() != targetConfiguration_->nAtoms()) &&
        !intramolecularForcesOnly_)
    {
        // Show warning message on array size mismatch
        if (status != GenericItem::ItemStatus::Created && velocities.size() != targetConfiguration_->nAtoms())
            Messenger::warn(
                "Size of existing velocities array doesn't match the current configuration size - they will be ignored.");

        Messenger::print("Random initial velocities will be assigned.\n");
        velocities.resize(targetConfiguration_->nAtoms(), Vector3());
        for (auto &&[v, iFree] : zip(velocities, free))
        {
            if (iFree)
                v.set(exp(DissolveMath::random() - 0.5), exp(DissolveMath::random() - 0.5), exp(DissolveMath::random() - 0.5));
            else
                v.zero();
            v /= sqrt(2.0 * M_PI);
        }
    }
    else if (intramolecularForcesOnly_)
    {
        Messenger::print("Only intramolecular forces will be calculated, so velocities will be zeroes.\n");
        velocities.resize(targetConfiguration_->nAtoms(), Vector3());
        std::fill(velocities.begin(), velocities.end(), Vector3());
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
    Vector3 vCom;
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
    if (trajectoryFrequency && trajectoryFrequency > 0)
    {
        std::string trajectoryFile = std::format("{}.md.xyz", targetConfiguration_->name());
        if ((!trajParser.appendOutput(trajectoryFile)) || (!trajParser.isFileGoodForWriting()))
        {
            Messenger::error("Failed to open MD trajectory output file '{}'.\n", trajectoryFile);
            return NodeConstants::ProcessResult::Failed;
        }
    }

    // Write header
    if (outputFrequency > 0)
    {
        Messenger::print("                                             Energies (kJ/mol)\n");
        Messenger::print("  Step             T(K)         Kinetic      Inter        Intra        Total      "
                         "deltaT(ps)\n");
    }

    // Start a timer
    Timer timer;

    // If we're not using a fixed timestep the forces need to be available immediately
    if (timestepType_ != TimestepType::Fixed)
    {
        // Zero force arrays
        std::fill(fUnbound.begin(), fUnbound.end(), Vector3());
        std::fill(fBound.begin(), fBound.end(), Vector3());

        auto potentialMap = kernel->potentialMap();

        if (targetMolecules.empty())
            ForcesModule::totalForces(targetConfiguration_, potentialMap,
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound);
        else
            ForcesModule::totalForces(targetConfiguration_, targetMolecules, potentialMap,
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound);

        // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        std::transform(fUnbound.begin(), fUnbound.end(), fUnbound.begin(), [](auto f) { return f * 100.0; });
        std::transform(fBound.begin(), fBound.end(), fBound.begin(), [](auto f) { return f * 100.0; });

        // Check for suitable timestep
        if (!determineTimeStep(timestepType_, fixedTimestep, fUnbound, fBound))
        {
            Messenger::print("Forces are currently too high for MD to proceed. Skipping this run.\n");
            return NodeConstants::ProcessResult::Unchanged;
        }
    }

    // Ready to do MD propagation of system
    auto step = 1;
    for (step = 1; step <= nSteps; ++step)
    {
        // Get timestep
        auto optDT = determineTimeStep(timestepType_, fixedTimestep, fUnbound, fBound);
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
        std::fill(fUnbound.begin(), fUnbound.end(), Vector3());
        std::fill(fBound.begin(), fBound.end(), Vector3());

        // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        if (targetMolecules.empty())
            ForcesModule::totalForces(targetConfiguration_, dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound);
        else
            ForcesModule::totalForces(targetConfiguration_, targetMolecules, dissolve().potentialMap(),
                                      intramolecularForcesOnly_ ? ForcesModule::ForceCalculationType::IntraMolecularFull
                                                                : ForcesModule::ForceCalculationType::Full,
                                      fUnbound, fBound);
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
        if (outputFrequency > 0 && (step == 1 || (step % outputFrequency == 0)))
        {
            // Include total energy term?
            if (energyFrequency > 0 && (step % energyFrequency == 0))
            {
                pePP = EnergyModule::pairPotentialEnergy(targetConfiguration_, dissolve().potentialMap());
                peBound = EnergyModule::intraMolecularEnergy(targetConfiguration_, dissolve().potentialMap());
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}\n", step,
                                 tInstant, ke, pePP.total(), peBound, ke + peBound + pePP.total(), dT);
            }
            else
                Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}                                          {:10.3e}\n", step,
                                 tInstant, ke, dT);
        }

        // Save trajectory frame
        if (trajectoryFrequency > 0 && (step % trajectoryFrequency == 0))
        {
            // Write number of atoms
            trajParser.writeLineF("{}\n", targetConfiguration_->nAtoms());

            // Construct and write header
            std::string header = std::format("Step {} of {}, T = {:10.3e}, ke = {:10.3e}", step, nSteps, tInstant, ke);
            if (energyFrequency && (step % energyFrequency == 0))
                header += std::format(", inter = {:10.3e}, intra = {:10.3e}, tot = {:10.3e}", pePP.total(), peBound,
                                      ke + pePP.total() + peBound);
            if (!trajParser.writeLine(header))
                return NodeConstants::ProcessResult::Failed;

            // Write Atoms
            for (const auto &i : atoms)
            {
                if (!trajParser.writeLineF("{:<3}   {:10.3f}  {:10.3f}  {:10.3f}\n", Elements::symbol(i.speciesAtom()->Z()),
                                           i.r().x, i.r().y, i.r().z))
                    return NodeConstants::ProcessResult::Failed;
            }
        }
    }
    timer.stop();

    // Close trajectory file
    if (trajectoryFrequency > 0)
        trajParser.closeFiles();

    if (capForces_)
        Messenger::print("A total of {} forces were capped over the course of the dynamics ({:9.3e} per step).\n", nCapped,
                         double(nCapped) / nSteps);
    Messenger::print("{} steps performed ({})\n", step - 1, timer.totalTimeString());

    // Increment configuration changeCount
    if (step > 1)
        targetConfiguration_->notifyAtomicPositionsChanged();

    /*
     * Calculation End
     */

    return NodeConstants::ProcessResult::Success;
}
