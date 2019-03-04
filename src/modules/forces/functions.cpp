/*
	*** Forces Module - Functions
	*** src/modules/forces/functions.cpp
	Copyright T. Youngs 2012-2019

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

/*
 * Total Forces on All Atoms
 */

// Calculate total intramolecular forces
void ForcesModule::intramolecularForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculate the total intramolecular forces within the system, arising from Bond, Angle, and Torsion
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

	// Loop over Bonds
	Bond** bonds = cfg->bonds().array();
	for (int m=start; m<cfg->nBonds(); m += stride) kernel.forces(bonds[m]);

	// Loop over Angles
	Angle** angles = cfg->angles().array();
	for (int m=start; m<cfg->nAngles(); m += stride) kernel.forces(angles[m]);

	// Loop over Torsions
	Torsion** torsions = cfg->torsions().array();
	for (int m=start; m<cfg->nTorsions(); m += stride) kernel.forces(torsions[m]);
}

// Calculate interatomic forces within the system
void ForcesModule::interatomicForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the interatomic forces in the system arising from contributions from PairPotential
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

// Calculate total forces within the system
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total forces within the system, arising from PairPotential interactions
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
	 * Calculate the total intramolecular forces within the system, arising from Bond, Angle, and Torsion
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

	// Loop over supplied atom indices
	const DynamicArray<Atom>& atoms = cfg->atoms();
	for (int n=start; n<targetIndices.nItems(); n += stride)
	{
		const Atom* i = atoms.constValue(targetIndices.at(n));

		// Calculate contributions from all bonds that this atom takes part in
		for (int m=0; m<i->bonds().nItems(); ++m) kernel.forces(i->bonds().at(m), i);

		// Calculate contributions from all angles that this atom takes part in
		for (int m=0; m<i->angles().nItems(); ++m) kernel.forces(i->angles().at(m), i);

		// Calculate contributions from all torsions that this atom takes part in
		for (int m=0; m<i->torsions().nItems(); ++m) kernel.forces(i->torsions().at(m), i);
	}
}

// Calculate interatomic forces on specified atoms within the specified Configuration
void ForcesModule::interatomicForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the interatomic forces in the system arising from contributions from PairPotential
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
		const Atom* i = atoms.constValue(targetIndices.at(n));

		kernel.forces(i, ProcessPool::subDivisionStrategy(strategy));
	}
}

// Calculate forces acting on specific atoms within the system (arising from all atoms)
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total forces acting on the supplied atom indices, arising from PairPotential interactions
	 * and intramolecular contributions from *all other atoms* in the system.
	 * 
	 * The supplied force arrays (fx, fy, and fz) should be initialised to the total number of atoms in the configuration,
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

// Calculate forces acting on specific Molecules within the system (arising from all atoms)
void ForcesModule::totalForces(ProcessPool& procPool, Configuration* cfg, const Array<Molecule*>& targetMolecules, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total forces acting on the supplied Molecules, arising from PairPotential interactions
	 * and intramolecular contributions from *all other atoms* in the system.
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
		Molecule* mol = targetMolecules.at(n);

		for (int i=0; i<mol->nAtoms(); ++i)
		{
			indices.add(mol->atom(i)->arrayIndex());
		}
	}

	// Call the atomic index-based function
	totalForces(procPool, cfg, indices, potentialMap, fx, fy, fz);
}
