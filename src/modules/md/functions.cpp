// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/species.h"
#include "data/atomicmasses.h"
#include "modules/forces/forces.h"
#include "modules/md/md.h"

// Cap forces in Configuration
int MDModule::capForces(double maxForce, std::vector<Vec3<double>> &f)
{
    double fMag;
    const auto maxForceSq = maxForce * maxForce;
    auto nCapped = 0;
    for (auto &fxyz : f)
    {
        fMag = fxyz.magnitudeSq();
        if (fMag < maxForceSq)
            continue;

        fMag = maxForce / sqrt(fMag);
        fxyz *= fMag;

        ++nCapped;
    }

    return nCapped;
}

// Determine timestep based on maximal force component
double MDModule::determineTimeStep(const std::vector<Vec3<double>> &f)
{
    auto fMax = *std::max_element(f.begin(), f.end(), [](auto &left, auto &right) { return left.absMax() < right.absMax(); });

    return 1.0 / fMax.absMax();
}

// Evolve Species coordinates, returning new coordinates
std::vector<Vec3<double>> MDModule::evolve(const ProcessPool &procPool, const PotentialMap &potentialMap, const Species *sp,
                                           double temperature, int nSteps, double deltaT,
                                           const std::vector<Vec3<double>> &rInit, std::vector<Vec3<double>> &velocities)
{
    assert(sp);
    assert(sp->nAtoms() == velocities.size());

    // Create arrays
    std::vector<double> mass(sp->nAtoms(), 0.0);
    std::vector<Vec3<double>> forces(sp->nAtoms()), accelerations(sp->nAtoms());

    // Variables
    auto &atoms = sp->atoms();
    double tInstant, ke, tScale;

    // Units
    // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
    // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
    const auto kb = 0.8314462;

    // Store atomic masses for future use
    for (auto &&[i, m] : zip(atoms, mass))
        m = AtomicMass::mass(i.Z());

    // Calculate total velocity and mass over all atoms
    Vec3<double> vCom;
    auto massSum = 0.0;
    for (auto &&[v, m] : zip(velocities, mass))
    {
        vCom += v * m;
        massSum += m;
    }

    // Remove any velocity shift
    vCom /= massSum;
    std::transform(velocities.begin(), velocities.end(), velocities.begin(), [vCom](auto vel) { return vel - vCom; });

    // Calculate instantaneous temperature
    ke = 0.0;
    for (auto &&[m, v] : zip(mass, velocities))
        ke += 0.5 * m * v.dp(v);
    tInstant = ke * 2.0 / (3.0 * atoms.size() * kb);

    // Rescale velocities for desired temperature
    tScale = sqrt(temperature / tInstant);
    std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto v) { return v * tScale; });

    // Get timestep
    auto deltaTSq = deltaT * deltaT;

    // Copy coordinates ready for propagation
    auto rNew = rInit;

    // Ready to do MD propagation of the species
    for (auto step = 1; step <= nSteps; ++step)
    {
        // Velocity Verlet first stage (A)
        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
        // B:  a(t+dt) = F(t+dt)/m
        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
        for (auto &&[r, v, a] : zip(rNew, velocities, accelerations))
        {
            // Propagate positions (by whole step)...
            r += v * deltaT + a * 0.5 * deltaTSq;

            // ...velocities (by half step)...
            v += a * 0.5 * deltaT;
        }

        // Zero force arrays
        std::fill(forces.begin(), forces.end(), Vec3<double>());

        // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
        ForcesModule::totalForces(procPool, sp, potentialMap, rNew, forces);
        std::transform(forces.begin(), forces.end(), forces.begin(), [](auto &f) { return f * 100.0; });

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
        tInstant = ke * 2.0 / (3.0 * sp->nAtoms() * kb);
        tScale = sqrt(temperature / tInstant);
        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto &v) { return v * tScale; });
    }

    return rNew;
}
