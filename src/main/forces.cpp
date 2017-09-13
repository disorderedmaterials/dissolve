/*
	*** dUQ - Forces
	*** src/main/forces.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/duq.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "base/timer.h"

// Calculate total intramolecular forces
void DUQ::intramolecularForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	ForceKernel kernel(procPool, cfg, potentialMap_, fx, fy, fz);

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
	int stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);

	// Loop over Bonds
	Bond** bonds = cfg->bonds();
	for (int m=start; m<cfg->nBonds(); m += stride) kernel.forces(bonds[m]);

	// Loop over Angles
	Angle** angles = cfg->angles();
	for (int m=start; m<cfg->nAngles(); m += stride) kernel.forces(angles[m]);

	// Loop over Torsions
	Torsion** torsions = cfg->torsions();
	for (int m=start; m<cfg->nTorsions(); m += stride) kernel.forces(torsions[m]);
}

// Calculate interatomic forces within the system
void DUQ::interatomicForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
	ForceKernel kernel(procPool, cfg, potentialMap_, fx, fy, fz);

	Cell* cell;

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(ProcessPool::OverGroups);
	int stride = procPool.interleavedLoopStride(ProcessPool::OverGroups);

	for (int cellId = start; cellId<cellArray.nCells(); cellId += stride)
	{
		cell = cellArray.cell(cellId);

		/*
		 * Calculation Begins
		 */

		// This cell with itself
		kernel.forces(cell, cell, false, true, ProcessPool::OverGroupProcesses);

		// Interatomic interactions between atoms in this cell and its neighbours
		kernel.forces(cell, true, ProcessPool::OverGroupProcesses);

		/*
		 * Calculation End
		 */
	}
}

// Calculate total forces within the system
void DUQ::totalForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total forces within the system, arising from inter-Grain PairPotential interactions
	 * and intramolecular contributions.
	 * 
	 * This is a serial routine (subroutines called from within are parallel).
	 */

	// Create a Timer
	Timer timer;
	
	// Calculate interatomic forces
	timer.start();
	interatomicForces(procPool, cfg, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do interatomic forces was %s.\n", timer.totalTimeString());
	
	// Calculate intramolecular forces
	timer.start();
	intramolecularForces(procPool, cfg, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do intramolecular forces was %s.\n", timer.totalTimeString());
	
	// Gather forces together over all processes
	if (!procPool.allSum(fx, cfg->nAtoms())) return;
	if (!procPool.allSum(fy, cfg->nAtoms())) return;
	if (!procPool.allSum(fz, cfg->nAtoms())) return;
}
