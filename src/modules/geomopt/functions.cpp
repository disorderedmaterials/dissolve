/*
    *** Geometry Optimisation Module - Functions
    *** src/modules/geomopt/functions.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        auto r = sp->atom(n)->r();
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
    double rmsf = 0.0;
    for (auto n = 0; n < xForce_.nItems(); ++n)
        rmsf += xForce_.constAt(n) * xForce_.constAt(n) + yForce_.constAt(n) * yForce_.constAt(n) +
                zForce_.constAt(n) * zForce_.constAt(n);
    rmsf /= xForce_.nItems();

    return sqrt(rmsf);
}

// Determine suitable step size from current forces
double GeometryOptimisationModule::gradientStepSize()
{
    double fMax = xForce_.maxAbs();
    double fTemp = yForce_.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;
    fTemp = zForce_.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;

    return 1.0e-5;
    return 1.0 / fMax;
}

// Sort bounds / energies so that minimum energy is in the central position
void GeometryOptimisationModule::sortBoundsAndEnergies(Vec3<double> &bounds, Vec3<double> &energies)
{
    // Ensure that the energy minimum is the midpoint
    auto minVal = energies.minElement();
    if (minVal != 1)
    {
        energies.swap(1, minVal);
        bounds.swap(1, minVal);
    }
}

// Return energy of adjusted coordinates, following the force vectors by the supplied amount
template <>
double GeometryOptimisationModule::energyAtGradientPoint(ProcessPool &procPool, Configuration *cfg,
                                                         const PotentialMap &potentialMap, double delta)
{
    Atom **atoms = cfg->atoms().array();
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
