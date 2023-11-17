// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

//#include "classes/configuration.h"
//#include "classes/species.h"
//#include "data/atomicMasses.h"
//#include "modules/forces/forces.h"
//#include "modules/dlPoly/dlPoly.h"
//
//// Cap forces in Configuration
//int DlPolyModule::capForces(double maxForce, std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra)
//{
//    double fMag;
//    const auto maxForceSq = maxForce * maxForce;
//    auto nCapped = 0;
//    for (auto &&[inter, intra] : zip(fInter, fIntra))
//    {
//        fMag = (inter + intra).magnitudeSq();
//        if (fMag < maxForceSq)
//            continue;
//
//        fMag = maxForce / sqrt(fMag);
//        inter *= fMag;
//        intra *= fMag;
//
//        ++nCapped;
//    }
//
//    return nCapped;
//}
//
//// Determine timestep to use
//std::optional<double> DlPolyModule::determineTimeStep(TimestepType timestepType, double requestedTimeStep,
//                                                  const std::vector<Vec3<double>> &fInter,
//                                                  const std::vector<Vec3<double>> &fIntra)
//{
//    if (timestepType == TimestepType::Fixed)
//        return requestedTimeStep;
//
//    // Simple variable timestep
//    if (timestepType == TimestepType::Variable)
//    {
//        auto absFMax = 0.0;
//        for (auto &&[inter, intra] : zip(fInter, fIntra))
//            absFMax = std::max(absFMax, (inter + intra).absMax());
//
//        return 1.0 / absFMax;
//    }
//
//    // Automatic timestep determination, using maximal interatomic force to guide the timestep up to the current fixed timestep
//    // value
//    auto absFMaxInter =
//        std::max_element(fInter.begin(), fInter.end(), [](auto &left, auto &right) { return left.absMax() < right.absMax(); })
//            ->absMax();
//
//    auto deltaT = 100.0 / absFMaxInter;
//    if (deltaT < (requestedTimeStep / 100.0))
//        return {};
//    return deltaT > requestedTimeStep ? requestedTimeStep : deltaT;
//}
//
//// Evolve Species coordinates, returning new coordinates
//std::vector<Vec3<double>> DlPolyModule::evolve(const ProcessPool &procPool, const PotentialMap &potentialMap, const Species *sp,
//                                           double temperature, int nSteps, double maxDeltaT,
//                                           const std::vector<Vec3<double>> &rInit, std::vector<Vec3<double>> &velocities)
//{
//    assert(sp);
//    assert(sp->nAtoms() == velocities.size());
//
//    // Create arrays
//    std::vector<double> mass(sp->nAtoms(), 0.0);
//    std::vector<Vec3<double>> fInter(sp->nAtoms()), fIntra(sp->nAtoms()), accelerations(sp->nAtoms());
//
//    // Variables
//    auto &atoms = sp->atoms();
//    double tInstant, ke, tScale;
//
//    // Units
//    // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
//    // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
//    const auto kb = 0.8314462;
//
//    // Store atomic masses for future use
//    std::transform(atoms.begin(), atoms.end(), mass.begin(), [](const auto &atom) { return AtomicMass::mass(atom.Z()); });
//
//    // Calculate total velocity and mass over all atoms
//    auto massSum = std::accumulate(mass.begin(), mass.end(), 0.0);
//    auto vCom = std::transform_reduce(velocities.begin(), velocities.end(), mass.begin(), Vec3<double>());
//
//    // Remove any velocity shift
//    vCom /= massSum;
//    std::transform(velocities.begin(), velocities.end(), velocities.begin(), [vCom](auto vel) { return vel - vCom; });
//
//    // Calculate instantaneous temperature
//    ke = std::transform_reduce(mass.begin(), mass.end(), velocities.begin(), 0.0, std::plus<>(),
//                               [](const auto m, const auto &v) { return 0.5 * m * v.dp(v); });
//    tInstant = ke * 2.0 / (3.0 * atoms.size() * kb);
//
//    // Rescale velocities for desired temperature
//    tScale = sqrt(temperature / tInstant);
//    std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto v) { return v * tScale; });
//
//    // Zero force arrays
//    std::fill(fInter.begin(), fInter.end(), Vec3<double>());
//    std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());
//
//    ForcesModule::totalForces(procPool, sp, potentialMap, ForcesModule::ForceCalculationType::Full, fInter, fIntra, rInit);
//
//    // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
//    std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto f) { return f * 100.0; });
//    std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto f) { return f * 100.0; });
//
//    // Check for suitable timestep
//    if (!determineTimeStep(TimestepType::Automatic, maxDeltaT, fInter, fIntra))
//    {
//        Messenger::print("Forces are currently too high for species MD to proceed. Try decreasing the maximum timestep.\n");
//        return rInit;
//    }
//
//    // Copy coordinates ready for propagation
//    auto rNew = rInit;
//
//    // Ready to do MD propagation of the species
//    for (auto step = 1; step <= nSteps; ++step)
//    {
//        // Get timestep
//        auto optDT = determineTimeStep(TimestepType::Automatic, maxDeltaT, fInter, fIntra);
//        if (!optDT)
//        {
//            Messenger::warn("A reasonable timestep could not be determined. Stopping evolution.\n");
//            break;
//        }
//        auto dT = *optDT;
//        auto deltaTSq = dT * dT;
//
//        // Velocity Verlet first stage (A)
//        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
//        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
//        // B:  a(t+dt) = F(t+dt)/m
//        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
//        for (auto &&[r, v, a] : zip(rNew, velocities, accelerations))
//        {
//            // Propagate positions (by whole step)...
//            r += v * dT + a * 0.5 * deltaTSq;
//
//            // ...velocities (by half step)...
//            v += a * 0.5 * dT;
//        }
//
//        // Zero force arrays
//        std::fill(fInter.begin(), fInter.end(), Vec3<double>());
//        std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());
//
//        // Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
//        ForcesModule::totalForces(procPool, sp, potentialMap, ForcesModule::ForceCalculationType::Full, fInter, fIntra, rNew);
//        std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto f) { return f * 100.0; });
//        std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto f) { return f * 100.0; });
//
//        // Velocity Verlet second stage (B) and velocity scaling
//        // A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
//        // A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
//        // B:  a(t+dt) = F(t+dt)/m
//        // B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
//        ke = 0.0;
//        for (auto &&[f1, f2, v, a, m] : zip(fInter, fIntra, velocities, accelerations, mass))
//        {
//            // Determine new accelerations
//            a = (f1 + f2) / m;
//
//            // ..and finally velocities again (by second half-step)
//            v += a * 0.5 * dT;
//
//            ke += 0.5 * m * v.dp(v);
//        }
//
//        // Rescale velocities for desired temperature
//        tInstant = ke * 2.0 / (3.0 * sp->nAtoms() * kb);
//        tScale = sqrt(temperature / tInstant);
//        std::transform(velocities.begin(), velocities.end(), velocities.begin(), [tScale](auto &v) { return v * tScale; });
//    }
//
//    return rNew;
//}
