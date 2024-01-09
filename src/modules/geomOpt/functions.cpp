// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "modules/energy/energy.h"
#include "modules/geomOpt/geomOpt.h"
#include "templates/algorithms.h"
#include <numeric>

// Copy coordinates from supplied Configuration into reference arrays
template <> void GeometryOptimisationModule::setReferenceCoordinates(Configuration *cfg)
{
    std::transform(cfg->atoms().begin(), cfg->atoms().end(), rRef_.begin(), [](const auto &i) { return i.r(); });
}

// Copy coordinates from supplied Species into reference arrays
template <> void GeometryOptimisationModule::setReferenceCoordinates(Species *sp)
{
    std::transform(sp->atoms().begin(), sp->atoms().end(), rRef_.begin(), [](const auto &i) { return i.r(); });
}

// Revert Configuration to reference coordinates
template <> void GeometryOptimisationModule::revertToReferenceCoordinates(Configuration *cfg)
{
    for (auto &&[ref, i] : zip(rRef_, cfg->atoms()))
        i.setCoordinates(ref);
}

// Revert Species to reference coordinates
template <> void GeometryOptimisationModule::revertToReferenceCoordinates(Species *sp)
{
    for (auto &&[ref, i] : zip(rRef_, sp->atoms()))
        sp->setAtomCoordinates(&i, ref);
}

// Return current RMS force
double GeometryOptimisationModule::rmsForce() const
{
    auto msf = std::accumulate(f_.begin(), f_.end(), 0.0,
                               [](auto &acc, const auto &f) { return acc + f.x * f.x + f.y * f.y + f.z * f.z; }) /
               f_.size();

    return sqrt(msf);
}

// Sort bounds / energies so that minimum energy is in the central position
void GeometryOptimisationModule::sortBoundsAndEnergies(std::array<double, 3> &bounds, std::array<double, 3> &energies)
{
    if (energies[0] < energies[1])
    {
        std::swap(energies[1], energies[0]);
        std::swap(bounds[1], bounds[0]);
    }
    if (energies[2] < energies[1])
    {
        std::swap(energies[1], energies[2]);
        std::swap(bounds[1], bounds[2]);
    }
}

// Return energy of adjusted coordinates, following the force vectors by the supplied amount
template <>
double GeometryOptimisationModule::energyAtGradientPoint(const ProcessPool &procPool, Configuration *cfg,
                                                         const PotentialMap &potentialMap, double delta)
{
    for (auto &&[i, r, f] : zip(cfg->atoms(), rRef_, f_))
        i.setCoordinates(r.x + f.x * delta, r.y + f.y * delta, r.z + f.z * delta);
    cfg->updateAtomLocations();

    return EnergyModule::totalEnergy(procPool, cfg, potentialMap);
}

// Return energy of adjusted coordinates, following the force vectors by the supplied amount
template <>
double GeometryOptimisationModule::energyAtGradientPoint(const ProcessPool &procPool, Species *sp,
                                                         const PotentialMap &potentialMap, double delta)
{
    for (auto &&[i, r, f] : zip(sp->atoms(), rRef_, f_))
        sp->setAtomCoordinates(&i, Vec3<double>(r.x + f.x * delta, r.y + f.y * delta, r.z + f.z * delta));

    return EnergyModule::totalEnergy(procPool, sp, potentialMap);
}

/*
 * Public Functions
 */

// Geometry optimise supplied Species
bool GeometryOptimisationModule::optimiseSpecies(const PotentialMap &potentialMap, const ProcessPool &procPool, Species *sp)
{
    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", maxCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {}.\n", initialStepSize_);
    Messenger::print("\n");

    // Initialise working arrays for coordinates and forces
    rRef_.resize(sp->nAtoms());
    rTemp_.resize(sp->nAtoms());
    f_.resize(sp->nAtoms());

    // Make sure the species' is ready for optimisation
    sp->setUpScaledInteractions();

    optimise<Species>(potentialMap, procPool, sp);

    return true;
}
