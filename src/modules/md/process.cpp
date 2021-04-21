// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/timer.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "classes/species.h"
#include "data/atomicmasses.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/md/md.h"
#include "templates/algorithms.h"

// Run main processing
bool MDModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Perform Molecular Dynamics on a given Configuration.
     *
     * This is a parallel routine, with processes operating in groups.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Get control parameters
    const auto capForce = keywords_.asBool("CapForces");
    const auto maxForce = keywords_.asDouble("CapForcesAt") * 100.0; // To convert from kJ/mol to 10 J/mol
    auto cutoffDistance = keywords_.asDouble("CutoffDistance");
    if (cutoffDistance < 0.0)
        cutoffDistance = dissolve.pairPotentialRange();
    auto deltaT = keywords_.asDouble("DeltaT");
    const auto energyFrequency = keywords_.asInt("EnergyFrequency");
    const auto nSteps = keywords_.asInt("NSteps");
    const auto outputFrequency = keywords_.asInt("OutputFrequency");
    auto randomVelocities = keywords_.asBool("RandomVelocities");
    const auto onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");
    const auto trajectoryFrequency = keywords_.asInt("TrajectoryFrequency");
    const auto variableTimestep = keywords_.asBool("VariableTimestep");
    auto writeTraj = trajectoryFrequency > 0;

    // Print argument/parameter summary
    Messenger::print("MD: Cutoff distance is {}\n", cutoffDistance);
    Messenger::print("MD: Number of steps = {}\n", nSteps);
    if (onlyWhenEnergyStable)
        Messenger::print("MD: Only peform MD if target Configuration energies are stable.\n");
    if (writeTraj)
        Messenger::print("MD: Trajectory file will be appended every {} step(s).\n", trajectoryFrequency);
    else
        Messenger::print("MD: Trajectory file off.\n");
    if (capForce)
        Messenger::print("MD: Forces will be capped to {:10.3e} kJ/mol per atom per axis.\n", maxForce / 100.0);
    if (energyFrequency > 0)
        Messenger::print("MD: Energy will be calculated every {} step(s).\n", energyFrequency);
    else
        Messenger::print("MD: Energy will be not be calculated.\n");
    if (outputFrequency > 0)
        Messenger::print("MD: Summary will be written every {} step(s).\n", outputFrequency);
    else
        Messenger::print("MD: Summary will not be written.\n");
    if (variableTimestep)
        Messenger::print("MD: Variable timestep will be employed.");
    else
        Messenger::print("MD: Constant timestep of {:e} ps will be used.\n", deltaT);
    if (restrictToSpecies_.nItems() > 0)
    {
        std::string speciesNames;
        for (Species *sp : restrictToSpecies_)
            speciesNames += fmt::format("  {}", sp->name());
        Messenger::print("MD: Calculation will be restricted to Species:{}\n", speciesNames);
    }
    Messenger::print("\n");

    for (Configuration *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        if (onlyWhenEnergyStable)
        {
            auto stabilityResult = EnergyModule::checkStability(dissolve.processingModuleData(), cfg);
            if (stabilityResult == EnergyModule::NotAssessable)
                return false;
            else if (stabilityResult == EnergyModule::EnergyUnstable)
            {
                Messenger::print("Skipping MD for Configuration '{}'.\n", cfg->niceName());
                continue;
            }
        }

        // Determine target Molecules (if there are entries in the targetSpecies_ list)
        Array<std::shared_ptr<Molecule>> targetMolecules;
        if (restrictToSpecies_.nItems() > 0)
        {
            for (auto n = 0; n < cfg->nMolecules(); ++n)
            {
                std::shared_ptr<Molecule> mol = cfg->molecule(n);
                if (restrictToSpecies_.contains(mol->species()))
                    targetMolecules.add(mol);
            }
        }

        // Get temperature from Configuration
        const auto temperature = cfg->temperature();

        // Create arrays
        std::vector<double> mass(cfg->nAtoms(), 0.0);
        std::vector<Vec3<double>> forces(cfg->nAtoms()), accelerations(cfg->nAtoms());

        // Variables
        auto nCapped = 0;
        auto &atoms = cfg->atoms();
        double tInstant, ke, tScale, peInter, peIntra;
        auto deltaTSq = deltaT * deltaT;

        /*
         * Calculation Begins
         */

        // Read in or assign random velocities
        // Realise the velocity array from the moduleData
        auto [velocities, status] = dissolve.processingModuleData().realiseIf<std::vector<Vec3<double>>>(
            fmt::format("{}//Velocities", cfg->niceName()), uniqueName(), GenericItem::NoFlags);
        if (status == GenericItem::ItemStatus::Created)
        {
            randomVelocities = true;
            velocities.resize(cfg->nAtoms(), Vec3<double>());
        }
        if (randomVelocities)
            Messenger::print("Random initial velocities will be assigned.\n");
        else
            Messenger::print("Existing velocities will be used.\n");

        // Initialise the random number buffer for all processes
        procPool.initialiseRandomBuffer(ProcessPool::PoolProcessesCommunicator);

        Vec3<double> vCom;
        auto massSum = 0.0;
        for (auto &&[i, v, m] : zip(atoms, velocities, mass))
        {
            if (randomVelocities)
            {
                v.x = exp(procPool.random() - 0.5) / sqrt(TWOPI);
                v.y = exp(procPool.random() - 0.5) / sqrt(TWOPI);
                v.z = exp(procPool.random() - 0.5) / sqrt(TWOPI);
            }

            // Grab atom mass for future use
            m = AtomicMass::mass(i->speciesAtom()->Z());

            // Calculate total velocity and mass over all atoms
            vCom += v * m;
            massSum += m;
        }

        // Remove any velocity shift
        vCom /= massSum;
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [vCom](auto vel) { return vel - vCom; });

        // Calculate instantaneous temperature
        // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
        // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
        const auto kb = 0.8314462;
        ke = 0.0;
        for (auto &&[m, v] : zip(mass, velocities))
            ke += 0.5 * m * v.dp(v);
        tInstant = ke * 2.0 / (3.0 * atoms.size() * kb);

        // Rescale velocities for desired temperature
        tScale = sqrt(temperature / tInstant);
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto v) { return v * tScale; });

        // Open trajectory file (if requested)
        LineParser trajParser;
        if (writeTraj)
        {
            std::string trajectoryFile = fmt::format("{}.md.xyz", cfg->name());
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
        if (outputFrequency > 0)
        {
            Messenger::print("                                             Energies (kJ/mol)\n");
            Messenger::print("  Step             T(K)         Kinetic      Inter        Intra        Total      "
                             "deltaT(ps)\n");
        }

        // Start a timer and reset the ProcessPool's time accumulator
        Timer timer;
        timer.start();
        procPool.resetAccumulatedTime();

        // Variable timestep requires forces to be available immediately
        if (variableTimestep)
        {
            if (restrictToSpecies_.nItems() > 0)
                ForcesModule::totalForces(procPool, cfg, targetMolecules, dissolve.potentialMap(), forces);
            else
                ForcesModule::totalForces(procPool, cfg, dissolve.potentialMap(), forces);

            // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
            std::transform(forces.begin(), forces.end(), forces.begin(), [](auto f) { return f * 100.0; });
        }

        // Ready to do MD propagation of system
        for (auto step = 1; step <= nSteps; ++step)
        {
            // Variable timestep?
            if (variableTimestep)
                deltaT = determineTimeStep(forces);

            // Velocity Verlet first stage (A)
            // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
            // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
            // B:  a(t+dt) = F(t+dt)/m
            // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
            for (auto &&[i, v, a] : zip(atoms, velocities, accelerations))
            {
                // Propagate positions (by whole step)...
                i->translateCoordinates(v * deltaT + a * 0.5 * deltaTSq);

                // ...velocities (by half step)...
                v += a * 0.5 * deltaT;
            }

            // Update Cell contents / Atom locations
            cfg->updateCellContents();

            // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
            if (restrictToSpecies_.nItems() > 0)
                ForcesModule::totalForces(procPool, cfg, targetMolecules, dissolve.potentialMap(), forces);
            else
                ForcesModule::totalForces(procPool, cfg, dissolve.potentialMap(), forces);
            std::transform(forces.begin(), forces.end(), forces.begin(), [](auto &f) { return f * 100.0; });

            // Cap forces
            if (capForce)
                nCapped = capForces(maxForce, forces);

            // Velocity Verlet second stage (B) and velocity scaling
            // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
            // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
            // B:  a(t+dt) = F(t+dt)/m
            // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
            ke = 0.0;
            for (auto &&[f, v, a, m] : zip(forces, velocities, accelerations, mass))
            {
                // Determine new accelerations
                a = f / m;

                // ..and finally velocities again (by second half-step)
                v += a * 0.5 * deltaT;

                ke += 0.5 * m * v.dp(v);
            }

            // Rescale velocities for desired temperature
            tInstant = ke * 2.0 / (3.0 * cfg->nAtoms() * kb);
            tScale = sqrt(temperature / tInstant);
            std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto &v) { return v * tScale; });

            // Convert ke from 10J/mol to kJ/mol
            ke *= 0.01;

            // Write step summary?
            if ((step == 1) || (step % outputFrequency == 0))
            {
                // Include total energy term?
                if ((energyFrequency > 0) && (step % energyFrequency == 0))
                {
                    peInter = EnergyModule::interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
                    peIntra = EnergyModule::intraMolecularEnergy(procPool, cfg, dissolve.potentialMap());
                    Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}   {:10.3e}\n", step,
                                     tInstant, ke, peInter, peIntra, ke + peIntra + peInter, deltaT);
                }
                else
                    Messenger::print("  {:<10d}    {:10.3e}   {:10.3e}                                          {:10.3e}\n",
                                     step, tInstant, ke, deltaT);
            }

            // Save trajectory frame
            if (writeTraj && (step % trajectoryFrequency == 0))
            {
                if (procPool.isMaster())
                {
                    // Write number of atoms
                    trajParser.writeLineF("{}\n", cfg->nAtoms());

                    // Construct and write header
                    std::string header = fmt::format("Step {} of {}, T = {:10.3e}, ke = {:10.3e}", step, nSteps, tInstant, ke);
                    if ((energyFrequency > 0) && (step % energyFrequency == 0))
                        header += fmt::format(", inter = {:10.3e}, intra = {:10.3e}, tot = {:10.3e}", peInter, peIntra,
                                              ke + peInter + peIntra);
                    if (!trajParser.writeLine(header))
                    {
                        procPool.decideFalse();
                        return false;
                    }

                    // Write Atoms
                    for (auto i : atoms)
                    {
                        if (!trajParser.writeLineF("{:<3}   {:10.3f}  {:10.3f}  {:10.3f}\n",
                                                   Elements::symbol(i->speciesAtom()->Z()), i->r().x, i->r().y, i->r().z))
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
        if (writeTraj && procPool.isMaster())
            trajParser.closeFiles();

        if (capForce)
            Messenger::print("A total of {} forces were capped over the course of the dynamics ({:9.3e} per step).\n", nCapped,
                             double(nCapped) / nSteps);
        Messenger::print("{} steps performed ({} work, {} comms)\n", nSteps, timer.totalTimeString(),
                         procPool.accumulatedTimeString());

        // Increment configuration changeCount
        cfg->incrementContentsVersion();

        /*
         * Calculation End
         */
    }

    return true;
}
