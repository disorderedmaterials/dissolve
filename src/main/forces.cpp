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

	int start, stride;

	// Set start/skip for parallel loop
	start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
	stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);

	// Main loop over molecules
	for (int m=start; m<cfg->nMolecules(); m += stride)
	{
		Molecule* mol = cfg->molecule(m);

		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next) kernel.forces(mol, b);

		// Angles
		for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next) kernel.forces(mol, a);
	}
}

// Calculate Grain forces within the system
void DUQ::grainForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Calculates the total Grain forces within the system, i.e. the energy contributions from PairPotential
	 * interactions between Grains. Any connections between Grains (which in reality correspond to proper chemical bonds
	 * between Atoms) are automatically excluded.
	 * 
	 * This is a parallel routine.
	 */

	// Initialise the Cell distributor
	const bool willBeModified = false, allowRepeats = false;
	cfg->initialiseCellDistribution();

	// Create a ForcesKernel
	ForceKernel kernel(procPool, cfg, potentialMap_, fx, fy, fz);

	int cellId, n, m, start, stride;
	Cell* cell;
	Grain* grainI, *grainJ;

	// Set start/skip for parallel loop
	start = procPool.interleavedLoopStart(ProcessPool::OverGroups);
	stride = procPool.interleavedLoopStride(ProcessPool::OverGroups);

	while (cellId = cfg->nextAvailableCell(procPool, willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			Messenger::printVerbose("Nothing for this process to do.\n");
			cfg->finishedWithCell(procPool, willBeModified, cellId);
			continue;
		}
		cell = cfg->cell(cellId);
		Messenger::printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nTotalCellNeighbours());

		/*
		 * Calculation Begins
		 */

		for (n=start; n<cell->nGrains(); n += stride)
		{
			grainI = cell->grain(n);

			// Inter-Grain interactions in this Cell...
			for (m=n+1; m<cell->nGrains(); ++m)
			{
				grainJ = cell->grain(m);
				kernel.forces(grainI, grainJ, false, false);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			kernel.forces(grainI, cell->nCellNeighbours(), cell->cellNeighbours(), false, true, ProcessPool::Individual);
			kernel.forces(grainI, cell->nMimCellNeighbours(), cell->mimCellNeighbours(), true, true, ProcessPool::Individual);
		}

		/*
		 * Calculation End
		 */
		
		// Must unlock the Cell when we are done with it!
		cfg->finishedWithCell(procPool, willBeModified, cellId);
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
	
	// Calculate Grain forces
	timer.start();
	grainForces(procPool, cfg, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do Grain forces was %s.\n", timer.totalTimeString());
	
	// Calculate intramolecular forces
	timer.start();
	intramolecularForces(procPool, cfg, fx, fy, fz);
	timer.stop();
	Messenger::printVerbose("Time to do intramolecular forces was %s.\n", timer.totalTimeString());
	
	// Gather forces together
	if (!procPool.allSum(fx, cfg->nAtoms())) return;
	if (!procPool.allSum(fy, cfg->nAtoms())) return;
	if (!procPool.allSum(fz, cfg->nAtoms())) return;
}
