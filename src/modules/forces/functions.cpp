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

#include "modules/forces/forces.h"
#include "classes/configuration.h"
#include "classes/forcekernel.h"
#include "classes/species.h"

/*
 * Total Forces on All Atoms
 */

// Calculate total intramolecular forces
void ForcesModule::intramolecularForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	ForceKernel kernel(procPool, cfg, potentialMap, fx, fy, fz);

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
	int stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

	// Loop over Molecules
	Molecule** molecules = cfg->molecules().array();
	const Molecule* mol;
	for (int m=start; m<cfg->nMolecules(); m += stride)
	{
		// Get Molecule pointer
		mol = molecules[m];

		// Loop over Bonds
		DynamicArrayConstIterator<SpeciesBond> bondIterator(mol->species()->constBonds());
		while (const SpeciesBond* b = bondIterator.iterate()) kernel.forces(b, mol->atom(b->indexI()), mol->atom(b->indexJ()));

		// Loop over Angles
		DynamicArrayConstIterator<SpeciesAngle> angleIterator(mol->species()->constAngles());
		while (const SpeciesAngle* a = angleIterator.iterate()) kernel.forces(a, mol->atom(a->indexI()), mol->atom(a->indexJ()), mol->atom(a->indexK()));

		// Loop over Torsions
		DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(mol->species()->constTorsions());
		while (const SpeciesTorsion* t = torsionIterator.iterate()) kernel.forces(t, mol->atom(t->indexI()), mol->atom(t->indexJ()), mol->atom(t->indexK()), mol->atom(t->indexL()));
	}
}

// Calculate interatomic forces within the supplied Configuration
void ForcesModule::interatomicForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the interatomic forces in the supplied Configuration arising from contributions from PairPotential
	 * interactions between individual atoms, and accounting for intramolecular terms
	 * 
	 * This is a parallel routine, with processes operating as process groups.
	 */

	// Grab the Cell array
	const CellArray& cellArray = cfg->cells();

	// Create a ForceKernel
	ForceKernel kernel(procPool, cfg, potentialMap, fx, fy, fz);

	Cell* cell;

	ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(strategy);
	int stride = procPool.interleavedLoopStride(strategy);

	for (int cellId = start; cellId<cellArray.nCells(); cellId += stride)
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

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	interatomicForces(procPool, cfg, potentialMap, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do interatomic forces was %s.\n", timer.totalTimeString());

	// Calculate intramolecular forces
	timer.start();
	intramolecularForces(procPool, cfg, potentialMap, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do intramolecular forces was %s.\n", timer.totalTimeString());

	// Gather forces together over all processes
	if (!procPool.allSum(fx, cfg->nAtoms())) return;
	if (!procPool.allSum(fy, cfg->nAtoms())) return;
	if (!procPool.allSum(fz, cfg->nAtoms())) return;
}

/*
 * Total Forces on Subset of Atoms
 */

// Calculate total intramolecular forces acting on specific atoms
void ForcesModule::intramolecularForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	ForceKernel kernel(procPool, cfg, potentialMap, fx, fy, fz);

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
	int stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

	// Loop over supplied atom indices
	const DynamicArray<Atom>& atoms = cfg->atoms();
	for (int n=start; n<targetIndices.nItems(); n += stride)
	{
		const Atom* i = atoms.constValue(targetIndices.constAt(n));
		const SpeciesAtom* spAtom = i->speciesAtom();
		const Molecule* mol = i->molecule();

		// Calcualte forces from SpeciesBond terms
		for (const auto* bond : spAtom->bonds())
		{
			kernel.forces(i, bond, mol->atom(bond->indexI()), mol->atom(bond->indexJ()));
		}

		// Add energy from SpeciesAngle terms
		for (const auto* angle : spAtom->angles())
		{
			kernel.forces(i, angle, mol->atom(angle->indexI()), mol->atom(angle->indexJ()), mol->atom(angle->indexK()));
		}

		// Add energy from SpeciesTorsion terms
		for (const auto* torsion : spAtom->torsions())
		{
			kernel.forces(i, torsion, mol->atom(torsion->indexI()), mol->atom(torsion->indexJ()), mol->atom(torsion->indexK()), mol->atom(torsion->indexL()));
		}
	}
}

// Calculate interatomic forces on specified atoms within the specified Configuration
void ForcesModule::interatomicForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the interatomic forces in the specified Configuration arising from contributions from PairPotential
	 * interactions between individual atoms, and accounting for intramolecular terms
	 * 
	 * This is a parallel routine, with processes operating as process groups.
	 */

	// Create a ForceKernel
	ForceKernel kernel(procPool, cfg, potentialMap, fx, fy, fz);

	ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(strategy);
	int stride = procPool.interleavedLoopStride(strategy);

	// Loop over supplied atom indices
	const DynamicArray<Atom>& atoms = cfg->atoms();
	for (int n=start; n<targetIndices.nItems(); n += stride)
	{
		const Atom* i = atoms.constValue(targetIndices.constAt(n));

		kernel.forces(i, ProcessPool::subDivisionStrategy(strategy));
	}
}

// Calculate forces acting on specific atoms within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total forces acting on the atom indices in the supplied Configuration, arising from PairPotential interactions
	 * and intramolecular contributions from *all other atoms* in the Configuration.
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
	interatomicForces(procPool, cfg, targetIndices, potentialMap, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do partial interatomic forces was %s.\n", timer.totalTimeString());

	// Calculate intramolecular forces
	timer.start();
	intramolecularForces(procPool, cfg, targetIndices, potentialMap, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do partial intramolecular forces was %s.\n", timer.totalTimeString());

	// Gather forces together over all processes
	if (!procPool.allSum(fx, cfg->nAtoms())) return;
	if (!procPool.allSum(fy, cfg->nAtoms())) return;
	if (!procPool.allSum(fz, cfg->nAtoms())) return;
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const Array<Molecule*>& targetMolecules, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	for (int n=0; n<targetMolecules.nItems(); ++n)
	{
		Molecule* mol = targetMolecules.constAt(n);

		for (int i=0; i<mol->nAtoms(); ++i)
		{
			indices.add(mol->atom(i)->arrayIndex());
		}
	}

	// Call the atomic index-based function
	totalForces(procPool, cfg, indices, potentialMap, fx, fy, fz);
}
