/*
	*** dUQ Methods - Forces
	*** src/lib/main/methods_forces.cpp
	Copyright T. Youngs 2012-2013

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
#include "classes/grain.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "base/comms.h"
#include "base/timer.h"

/*!
 * \brief Calculate the total intramolecular forces within the system
 * \details Calculate the total intramolecular forces within the system, arising from Bond, Angle, and Torsion
 * terms in all molecules.
 * 
 * This is a parallel routine..
 */
void DUQ::intramolecularForces(Configuration& cfg, double* fx, double* fy, double* fz, DUQComm::CommGroup group)
{
	double distance, angle, force, dp, magji, magjk;
	int index, start, stride;
	Vec3<double> vecji, vecjk, forcei, forcek;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	// Bond forces
	Bond* b;
	for (int n=start; n<cfg.nBonds(); n += stride)
	{
		b = cfg.bond(n);

		// Determine whether we need to apply minimum image to the vector calculation
		if (cfg.box()->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) vecji = cfg.box()->minimumVector(b->i(), b->j());
		else vecji = b->j()->r() - b->i()->r();
		
		// Get distance and normalise vector ready for force calculation
		distance = vecji.magAndNormalise();

		// Determine final forces
		vecji *= b->force(distance);

		// Calculate forces
		index = b->i()->index();
		fx[index] -= vecji.x;
		fy[index] -= vecji.y;
		fz[index] -= vecji.z;
		index = b->j()->index();
		fx[index] += vecji.x;
		fy[index] += vecji.y;
		fz[index] += vecji.z;
	}

	// Angle forces
	Angle* a;
	for (int n=start; n<cfg.nAngles(); n += stride)
	{
		a = cfg.angle(n);
		
		// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
		if (cfg.box()->useMim(a->j()->grain()->cell(), a->i()->grain()->cell())) vecji = cfg.box()->minimumVector(a->j(), a->i());
		else vecji = a->i()->r() - a->j()->r();
		if (cfg.box()->useMim(a->j()->grain()->cell(), a->k()->grain()->cell())) vecjk = cfg.box()->minimumVector(a->j(), a->k());
		else vecjk = a->k()->r() - a->j()->r();
		
		// Calculate angle
		magji = vecji.magAndNormalise();
		magjk = vecjk.magAndNormalise();
		angle = Box::angle(vecji, vecjk, dp);

		// Determine Angle force vectors for atoms
		force = a->force(angle);
		forcei = vecjk - vecji * dp;
		forcei *= force / magji;
		forcek = vecji - vecjk * dp;
		forcek *= force / magjk;
		
		// Store forces
		index = a->i()->index();
		fx[index] += forcei.x;
		fy[index] += forcei.y;
		fz[index] += forcei.z;
		index = a->j()->index();
		fx[index] -= forcei.x + forcek.x;
		fy[index] -= forcei.y + forcek.y;
		fz[index] -= forcei.z + forcek.z;
		index = a->k()->index();
		fx[index] += forcek.x;
		fy[index] += forcek.y;
		fz[index] += forcek.z;
	}
}

/*!
 * \brief Calculate Grain forces within the system
 * \details Calculates the total Grain forces within the system, i.e. the energy contributions from PairPotential
 * interactions between Grains. Any connections between Grains (which in reality correspond to proper chemical bonds
 * between Atoms) are automatically excluded.
 * 
 * This is a parallel routine.
 */
void DUQ::grainForces(Configuration& cfg, double* fx, double* fy, double* fz, double cutoffSq, DUQComm::CommGroup group)
{
	// Initialise the Cell distributor
	const bool willBeModified = FALSE, allowRepeats = FALSE;
	cfg.initialiseCellDistribution();

	// Create a ForcesKernel
	ForceKernel kernel(cfg.box(), potentialMap_);

	int cellId, n, m, start, stride;
	Cell* cell;
	Grain* grainI, *grainJ;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	while (cellId = cfg.nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			msg.printVerbose("Nothing for this process to do.\n");
			cfg.finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg.box()->cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nNeighbours());

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
				kernel.forces(grainI, grainJ, cutoffSq, FALSE, FALSE, fx, fy, fz);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			kernel.forces(grainI, cell->neighbours(), cutoffSq, TRUE, fx, fy, fz, DUQComm::Solo);
		}

		/*
		 * Calculation End
		 */
		
		// Must unlock the Cell when we are done with it!
		cfg.finishedWithCell(willBeModified, cellId);
	}
}

/*!
 * \brief Calculate total forces within the system
 * \details Calculates the total forces within the system, arising from inter-Grain PairPotential interactions
 * and intramolecular contributions.
 * 
 * This is a serial routine (subroutines called from within are parallel).
 */
void DUQ::totalForces(Configuration& cfg, double* fx, double* fy, double* fz, double cutoffSq, DUQComm::CommGroup group)
{
	// Create a Timer
	Timer timer;
	
	// Calculate Grain forces
	timer.start();
	grainForces(cfg, fx, fy, fz, cutoffSq, group);
	timer.stop();
	msg.printVerbose("Time to do Grain forces was %s.\n", timer.timeString());
	
	// Calculate intramolecular forces
	timer.start();
	intramolecularForces(cfg, fx, fy, fz, group);
	timer.stop();
	msg.printVerbose("Time to do intramolecular forces was %s.\n", timer.timeString());
	
	// Gather forces together
	if (group != DUQComm::Solo)
	{
		if (!Comm.allSum(fx, cfg.nAtoms())) return;
		if (!Comm.allSum(fy, cfg.nAtoms())) return;
		if (!Comm.allSum(fz, cfg.nAtoms())) return;
	}
}
