// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "modules/energy/energy.h"
#include "modules/geomopt/geomopt.h"

// Copy coordinates from supplied Configuration into reference arrays
template <> void GeometryOptimisationModule::setReferenceCoordinates(Configuration *cfg)
{
    for (auto n = 0; n < cfg->nAtoms(); ++n)
    {
        auto r = cfg->atom(n)->r();
        xRef_[n] = r.x;
        yRef_[n] = r.y;
        zRef_[n] = r.z;
    }
}

// Copy coordinates from supplied Species into reference arrays
template <> void GeometryOptimisationModule::setReferenceCoordinates(Species *sp)
{
    for (auto n = 0; n < sp->nAtoms(); ++n)
    {
        auto r = sp->atom(n).r();
        xRef_[n] = r.x;
        yRef_[n] = r.y;
        zRef_[n] = r.z;
    }
}

// Revert Configuration to reference coordinates
template <> void GeometryOptimisationModule::revertToReferenceCoordinates(Configuration *cfg)
{
    for (auto n = 0; n < cfg->nAtoms(); ++n)
        cfg->atom(n)->setCoordinates(xRef_[n], yRef_[n], zRef_[n]);
}

// Revert Species to reference coordinates
template <> void GeometryOptimisationModule::revertToReferenceCoordinates(Species *sp)
{
    for (auto n = 0; n < sp->nAtoms(); ++n)
        sp->setAtomCoordinates(n, xRef_[n], yRef_[n], zRef_[n]);
}

// Return current RMS force
double GeometryOptimisationModule::rmsForce() const
{
    auto rmsf = 0.0;

    for (auto n = 0; n < xForce_.nItems(); ++n)
        rmsf += xForce_.at(n) * xForce_.at(n) + yForce_.at(n) * yForce_.at(n) + zForce_.at(n) * zForce_.at(n);
    rmsf /= xForce_.nItems();

    return sqrt(rmsf);
}

// Determine suitable step size from current forces
double GeometryOptimisationModule::gradientStepSize()
{
    auto fMax = xForce_.maxAbs();
    auto fTemp = yForce_.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;
    fTemp = zForce_.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;

    return 1.0e-5;
    return 1.0 / fMax;
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
double GeometryOptimisationModule::energyAtGradientPoint(ProcessPool &procPool, Configuration *cfg,
                                                         const PotentialMap &potentialMap, double delta)
{
    auto &atoms = cfg->atoms();
    for (auto n = 0; n < cfg->nAtoms(); ++n)
        atoms[n]->setCoordinates(xRef_[n] + xForce_[n] * delta, yRef_[n] + yForce_[n] * delta, zRef_[n] + zForce_[n] * delta);
    cfg->updateCellContents();

    return EnergyModule::totalEnergy(procPool, cfg, potentialMap);
}

// Return energy of adjusted coordinates, following the force vectors by the supplied amount
template <>
double GeometryOptimisationModule::energyAtGradientPoint(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                                         double delta)
{
    for (auto n = 0; n < sp->nAtoms(); ++n)
        sp->setAtomCoordinates(n, xRef_[n] + xForce_[n] * delta, yRef_[n] + yForce_[n] * delta, zRef_[n] + zForce_[n] * delta);

    return EnergyModule::totalEnergy(procPool, sp, potentialMap);
}

/*
 * Public Functions
 */

// Geometry optimise supplied Species
bool GeometryOptimisationModule::optimiseSpecies(Dissolve &dissolve, ProcessPool &procPool, Species *sp)
{
    // Retrieve Module options
    nCycles_ = keywords_.asInt("NCycles");
    tolerance_ = keywords_.asDouble("Tolerance");
    initialStepSize_ = keywords_.asDouble("StepSize");

    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", nCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {}.\n", initialStepSize_);
    Messenger::print("\n");

    // Initialise working arrays for coordinates and forces
    xRef_.initialise(sp->nAtoms());
    yRef_.initialise(sp->nAtoms());
    zRef_.initialise(sp->nAtoms());
    xTemp_.initialise(sp->nAtoms());
    yTemp_.initialise(sp->nAtoms());
    zTemp_.initialise(sp->nAtoms());
    xForce_.initialise(sp->nAtoms());
    yForce_.initialise(sp->nAtoms());
    zForce_.initialise(sp->nAtoms());

    optimise<Species>(dissolve, procPool, sp);

    return true;
}
