/*
    *** Forces Module - Functions
    *** src/modules/forces/functions.cpp
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

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/forcekernel.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "modules/forces/forces.h"

// Calculate interatomic forces within the supplied Configuration
void ForcesModule::interAtomicForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                     Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculates the interatomic forces in the supplied Configuration arising from contributions from PairPotential
     * interactions between individual atoms, and accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Grab the Cell array
    const CellArray &cellArray = cfg->cells();

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap, fx, fy, fz);

    Cell *cell;

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    for (int cellId = start; cellId < cellArray.nCells(); cellId += stride)
    {
        cell = cellArray.cell(cellId);

        /*
         * Calculation Begins
         */

        // This cell with itself
        kernel.forces(cell, cell, false, true, ProcessPool::subDivisionStrategy(strategy));

        // Interatomic interactions between atoms in this cell and its neighbours
        kernel.forces(cell, true, ProcessPool::subDivisionStrategy(strategy));

        /*
         * Calculation End
         */
    }
}

// Calculate interatomic forces on specified atoms within the specified Configuration
void ForcesModule::interAtomicForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                                     const PotentialMap &potentialMap, Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculates the interatomic forces in the specified Configuration arising from contributions from PairPotential
     * interactions between individual atoms, and accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap, fx, fy, fz);

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    // Loop over supplied atom indices
    const DynamicArray<Atom> &atoms = cfg->atoms();
    for (int n = start; n < targetIndices.nItems(); n += stride)
    {
        const Atom *i = atoms.constValue(targetIndices.constAt(n));

        kernel.forces(i, ProcessPool::subDivisionStrategy(strategy));
    }
}

// Calculate interatomic forces within the specified Species
void ForcesModule::interAtomicForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap, Array<double> &fx,
                                     Array<double> &fy, Array<double> &fz)
{
    double scale, r, magjisq;
    const auto cutoffSq = potentialMap.range() * potentialMap.range();
    Vec3<double> vecij;
    SpeciesAtom *j;
    // NOTE PR #334 : use for_each_pair
    for (auto indexI = 0; indexI < sp->nAtoms() - 1; ++indexI)
    {
        auto *i = sp->atom(indexI);

        for (int indexJ = indexI + 1; indexJ < sp->nAtoms(); ++indexJ)
        {
            j = sp->atom(indexJ);

            // Get intramolecular scaling of atom pair
            scale = i->scaling(j);
            if (scale < 1.0e-3)
                continue;

            // Determine final forces
            vecij = j->r() - i->r();
            magjisq = vecij.magnitudeSq();
            if (magjisq > cutoffSq)
                continue;
            r = sqrt(magjisq);
            vecij /= r;

            vecij *= potentialMap.force(i, j, r) * scale;
            fx[indexI] += vecij.x;
            fy[indexI] += vecij.y;
            fz[indexI] += vecij.z;
            fx[indexJ] -= vecij.x;
            fy[indexJ] -= vecij.y;
            fz[indexJ] -= vecij.z;
        }
    }
}

// Calculate total intramolecular forces acting on specific atoms in Configuration
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                                        const PotentialMap &potentialMap, Array<double> &fx, Array<double> &fy,
                                        Array<double> &fz)
{
    /*
     * Calculate the total intramolecular forces for the specific atoms provided, arising from Bond, Angle, and Torsion
     * terms.
     *
     * Calculated forces are added in to the provided arrays. Assembly of the arrays over processes must be performed by the
     * calling function.
     *
     * This is a parallel routine.
     */

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap, fx, fy, fz);

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    // Loop over supplied atom indices
    const DynamicArray<Atom> &atoms = cfg->atoms();
    for (int n = start; n < targetIndices.nItems(); n += stride)
    {
        const Atom *i = atoms.constValue(targetIndices.constAt(n));
        const SpeciesAtom *spAtom = i->speciesAtom();
        std::shared_ptr<const Molecule> mol = i->molecule();

        // Calculate forces from SpeciesBond terms
        for (const SpeciesBond &bond : spAtom->bonds())
            kernel.forces(i, bond, mol->atom(bond.indexI()), mol->atom(bond.indexJ()));

        // Calculate forces from SpeciesAngle terms
        for (const SpeciesAngle &angle : spAtom->angles())
            kernel.forces(i, angle, mol->atom(angle.indexI()), mol->atom(angle.indexJ()), mol->atom(angle.indexK()));

        // Calculate forces from SpeciesTorsion terms
        for (const SpeciesTorsion &torsion : spAtom->torsions())
            kernel.forces(i, torsion, mol->atom(torsion.indexI()), mol->atom(torsion.indexJ()), mol->atom(torsion.indexK()),
                          mol->atom(torsion.indexL()));

        // Calculate forces from SpeciesImproper terms
        for (const SpeciesImproper &improper : spAtom->impropers())
            kernel.forces(i, improper, mol->atom(improper.indexI()), mol->atom(improper.indexJ()), mol->atom(improper.indexK()),
                          mol->atom(improper.indexL()));
    }
}

// Calculate total intramolecular forces in Configuration
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                        Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculate the total intramolecular forces within the supplied Configuration, arising from Bond, Angle, and Torsion
     * terms in all molecules.
     *
     * Calculated forces are added in to the provided arrays. Assembly of the arrays over processes must be performed by the
     * calling function.
     *
     * This is a parallel routine.
     */

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap, fx, fy, fz);

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    // Loop over Molecules
    std::deque<std::shared_ptr<Molecule>> molecules = cfg->molecules();
    std::shared_ptr<const Molecule> mol;
    for (int m = start; m < cfg->nMolecules(); m += stride)
    {
        // Get Molecule pointer
        mol = molecules[m];

        // Loop over bonds
        for (const auto &bond : mol->species()->constBonds())
            kernel.forces(bond, mol->atom(bond.indexI()), mol->atom(bond.indexJ()));

        // Loop over angles
        for (const auto &angle : mol->species()->constAngles())
            kernel.forces(angle, mol->atom(angle.indexI()), mol->atom(angle.indexJ()), mol->atom(angle.indexK()));

        // Loop over torsions
        for (const auto &torsion : mol->species()->constTorsions())
            kernel.forces(torsion, mol->atom(torsion.indexI()), mol->atom(torsion.indexJ()), mol->atom(torsion.indexK()),
                          mol->atom(torsion.indexL()));

        // Loop over impropers
        for (const auto &imp : mol->species()->constImpropers())
            kernel.forces(imp, mol->atom(imp.indexI()), mol->atom(imp.indexJ()), mol->atom(imp.indexK()),
                          mol->atom(imp.indexL()));
    }
}

// Calculate total intramolecular forces in Species
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap, Array<double> &fx,
                                        Array<double> &fy, Array<double> &fz)
{
    NonPeriodicBox box(1.0);
    ForceKernel kernel(procPool, &box, potentialMap, fx, fy, fz);

    // Loop over bonds
    for (const auto &b : sp->constBonds())
        kernel.forces(b);

    // Loop over angles
    for (const auto &a : sp->constAngles())
        kernel.forces(a);

    // Loop over torsions
    for (const auto &t : sp->constTorsions())
        kernel.forces(t);

    // Loop over impropers
    for (const auto &imp : sp->constImpropers())
        kernel.forces(imp);
}

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap, Array<double> &fx,
                               Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculates the total forces within the supplied Configuration, arising from PairPotential interactions
     * and intramolecular contributions.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Create a Timer
    Timer timer;

    // Calculate interatomic forces
    timer.start();
    interAtomicForces(procPool, cfg, potentialMap, fx, fy, fz);
    timer.stop();
    Messenger::printVerbose("Time to do interatomic forces was {}.\n", timer.totalTimeString());

    // Calculate intramolecular forces
    timer.start();
    intraMolecularForces(procPool, cfg, potentialMap, fx, fy, fz);
    timer.stop();
    Messenger::printVerbose("Time to do intramolecular forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    if (!procPool.allSum(fx, cfg->nAtoms()))
        return;
    if (!procPool.allSum(fy, cfg->nAtoms()))
        return;
    if (!procPool.allSum(fz, cfg->nAtoms()))
        return;
}

// Calculate forces acting on specific atoms within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg, const Array<int> &targetIndices,
                               const PotentialMap &potentialMap, Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculates the total forces acting on the atom indices in the supplied Configuration, arising from PairPotential
     * interactions and intramolecular contributions from *all other atoms* in the Configuration.
     *
     * The supplied force arrays (fx, fy, and fz) should be initialised to the total number of atoms in the Configuration,
     * rather than the number of atoms in the targetIndices list.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Create a Timer
    Timer timer;

    // Calculate interatomic forces
    timer.start();
    interAtomicForces(procPool, cfg, targetIndices, potentialMap, fx, fy, fz);
    timer.stop();
    Messenger::printVerbose("Time to do partial interatomic forces was {}.\n", timer.totalTimeString());

    // Calculate intramolecular forces
    timer.start();
    intraMolecularForces(procPool, cfg, targetIndices, potentialMap, fx, fy, fz);
    timer.stop();
    Messenger::printVerbose("Time to do partial intramolecular forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    if (!procPool.allSum(fx, cfg->nAtoms()))
        return;
    if (!procPool.allSum(fy, cfg->nAtoms()))
        return;
    if (!procPool.allSum(fz, cfg->nAtoms()))
        return;
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg,
                               const Array<std::shared_ptr<Molecule>> &targetMolecules, const PotentialMap &potentialMap,
                               Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    /*
     * Calculates the total forces acting on the supplied Molecules, arising from PairPotential interactions
     * and intramolecular contributions from *all other atoms* in the Configuration.
     *
     * The supplied force arrays (fx, fy, and fz) should be initialised to the total number of atoms in the configuration,
     * rather than the number of atoms in the targetIndices list.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Convert the Molecule array into an array of atoms
    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    Array<int> indices;
    for (int n = 0; n < targetMolecules.nItems(); ++n)
    {
        std::shared_ptr<Molecule> mol = targetMolecules.constAt(n);

        for (int i = 0; i < mol->nAtoms(); ++i)
        {
            indices.add(mol->atom(i)->arrayIndex());
        }
    }

    // Call the atomic index-based function
    totalForces(procPool, cfg, indices, potentialMap, fx, fy, fz);
}

// Calculate total forces within the specified Species
void ForcesModule::totalForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap, Array<double> &fx,
                               Array<double> &fy, Array<double> &fz)
{
    interAtomicForces(procPool, sp, potentialMap, fx, fy, fz);

    intraMolecularForces(procPool, sp, potentialMap, fx, fy, fz);
}
