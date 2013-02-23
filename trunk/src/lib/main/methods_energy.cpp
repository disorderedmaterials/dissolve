/*
	*** dUQ Methods - Energy
	*** src/lib/main/methods_energy.cpp
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
#include "classes/energykernel.h"
#include "classes/molecule.h"
#include "base/comms.h"
#include "base/timer.h"

/*!
 * \brief Return total intramolecular energy of the system
 * \details Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
 * terms in all molecules.
 * 
 * This is a parallel routine, with processes operating as a standard world group.
 */
double DUQ::intramolecularEnergy(Configuration& cfg)
{
	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	double energy = 0.0;
	int start, stride;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Bond energy / corrections
	Bond* b;
	for (int n=start; n<cfg.nBonds(); n += stride)
	{
		b = cfg.bond(n);
		energy += kernel.energy(b);
	}

	// Angle energy / corrections
	Angle* a;
	for (int n=start; n<cfg.nAngles(); n += stride)
	{
		a = cfg.angle(n);
		energy += kernel.energy(a);
	}

	msg.printVerbose("Intramolecular Energy (Local) is %15.9e\n", energy);
	
	// Sum energy and print
	Comm.allSum(&energy, 1);
	msg.printVerbose("Intramolecular Energy (World) is %15.9e\n", energy);

	return energy;
}

/*!
 * \brief Return Grain energy of the system
 * \details Calculates the total Grain energy of the system, i.e. the energy contributions from PairPotential
 * interactions between Grains.
 * 
 * This is a parallel routine, with processes operating as process groups.
 */
double DUQ::grainEnergy(Configuration& cfg)
{
	// Initialise the Cell distributor
	const bool willBeModified = FALSE, allowRepeats = FALSE;
	cfg.initialiseCellDistribution();

	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	int cellId, n, m, start, stride;
	Cell* cell;
	Grain* grainI, *grainJ;
	double totalEnergy = 0.0;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::Group);
	stride = Comm.interleavedLoopStride(DUQComm::Group);

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
		
		// Loop over Grains
		// Parallelism employed in the outer loop - as such, DUQComm::Solo is passed to the Grain/Cell method.
		for (n=start; n<cell->nGrains(); n += stride)
		{
			grainI = cell->grain(n);

			// Inter-Grain interactions in this Cell...
			for (m=n+1; m<cell->nGrains(); ++m)
			{
				grainJ = cell->grain(m);
				totalEnergy += kernel.energy(grainI, grainJ, pairPotentialRangeSquared_, FALSE, FALSE);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			totalEnergy += kernel.energy(grainI, cell->neighbours(), pairPotentialRangeSquared_, TRUE, DUQComm::Solo);
		}

		/*
		 * Calculation End
		 */
		// Must unlock the Cell when we are done with it!
		cfg.finishedWithCell(willBeModified, cellId);
	}
	msg.printVerbose("Grain Energy (Local) is %15.9e\n", totalEnergy);
	
	// Sum energy and print
	Comm.allSum(&totalEnergy, 1);
	msg.printVerbose("Grain Energy (World) is %15.9e\n", totalEnergy);

	return totalEnergy;
}

/*!
 * \brief Return total energy of the system
 * \details Calculates the total energy of the entire system.
 * 
 * This is a serial routine (subroutines called from within are parallel).
 */
double DUQ::totalEnergy(Configuration& cfg)
{
	msg.print("Calculating total energy...\n");
	
	double eGrain, eIntra;
	
	// Calculate Grain energy
	Timer grainTimer;
	eGrain = grainEnergy(cfg);
	grainTimer.stop();
	
	// Calculate intramolecular and interGrain correction energy
	Timer intraTimer;
	eIntra = intramolecularEnergy(cfg);
	intraTimer.stop();

	msg.print("Time to do Grain energy was %s, intramolecular energy was %s.\n", grainTimer.timeString(), intraTimer.timeString());

	msg.print("Total Energy (World) is %15.9e (%15.9e Grain + %15.9e Intramolecular)\n", eGrain+eIntra, eGrain, eIntra);
	return eGrain+eIntra;
}

/*!
 * \brief Test - Return total intramolecular (+correction) energy
 * \details Calculate the total intramolecular energy of the system.
 * 
 * This is a serial routine, with all processes independently calculating their own value.
 */
double DUQ::intramolecularEnergyTest(Configuration& cfg)
{
	double distanceSq, angle;
	double intraEnergy = 0.0;

	// Bond energy / corrections
	Bond* b;
	for (int n=0; n<cfg.nBonds(); ++n)
	{
		b = cfg.bond(n);

		distanceSq = cfg.box()->minimumDistanceSquared(b->i(), b->j());

		// Determine Bond energy
		intraEnergy += b->energy(sqrt(distanceSq));
	}

	// Angle energy / corrections
	Angle* a;
	Vec3<double> vecji, vecjk;
	for (int n=0; n<cfg.nAngles(); ++n)
	{
		a = cfg.angle(n);
		
		// Gget vectors 'j-i' and 'j-k'
		vecji = cfg.box()->minimumVector(a->j(), a->i());
		vecjk = cfg.box()->minimumVector(a->j(), a->k());
		
		// Calculate angle
		vecji.normalise();
		vecjk.normalise();
		angle = Box::angle(vecji, vecjk);

		// Determine Angle energy
		intraEnergy += a->energy(angle);
	}
	
	return intraEnergy;
}

/*!
 * \brief Test - Return total energy of the system
 * \details Calculate the total energy of the system using a basic loops, with each
 * process calculating its own value.
 * 
 * This is a serial routine, with all processes independently calculating their own value.
 */
double DUQ::totalEnergyTest(Configuration& cfg)
{
	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	/*
	 * Calculation Begins
	 */

	printf("ppsq = %f\n", pairPotentialRangeSquared_);
	int n, m;
	Grain* grainI, *grains = cfg.grains();
	double grainEnergy = 0.0, intraEnergy = 0.0;
	for (n=0; n<cfg.nGrains()-1; ++n)
	{
		grainI = &grains[n];

		for (m=n+1; m<cfg.nGrains(); ++m) grainEnergy += kernel.energy(grainI, &grains[m], pairPotentialRangeSquared_, TRUE, FALSE);
	}

	// Calculate intramolecular energy and correction to inter-Grain energy
	intraEnergy = intramolecularEnergyTest(cfg);

	/*
	 * Calculation End
	 */
	
	msg.printVerbose("Total Grain Energy (TEST) is %15.9e\n", grainEnergy);
	msg.printVerbose("Total Intramolecular Energy (TEST) is %15.9e\n", intraEnergy);
	msg.printVerbose("Total Energy (TEST) is %15.9e\n", grainEnergy + intraEnergy);
	return grainEnergy + intraEnergy;
}

/*!
 * \brief Test - Return total energy of the system
 * \details Calculate the total energy of the system in the Cell-based manner used by the main
 * total energy routine.
 * 
 * This is a serial routine, with all processes independently calculating their own value.
 */
double DUQ::totalEnergyTestCells(Configuration& cfg)
{
	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	int cellId, n, m;
	Cell* cell;
	Grain* grainI, *grainJ;
	double totalEnergy = 0.0;
	for (cellId=0; cellId<cfg.box()->nCells(); ++cellId)
	{
		cell = cfg.box()->cell(cellId);

		/*
		 * Calculation Begins
		 */
		
		// Inter-Grain interactions in this Cell...
		for (n=0; n<cell->nGrains(); ++n)
		{
			grainI = cell->grain(n);
			for (m=n+1; m<cell->nGrains(); ++m)
			{
				grainJ = cell->grain(m);
				
				totalEnergy += kernel.energy(grainI, grainJ, FALSE, FALSE);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			totalEnergy += kernel.energy(grainI, cell->neighbours(), TRUE);
		}

		/*
		 * Calculation End
		 */
	}

	// Calculate intramolecular energy and correction to inter-Grain energy
	double intraEnergy = intramolecularEnergyTest(cfg);

	msg.printVerbose("Total Grain Energy (TESTCELL) is %15.9e\n", totalEnergy);
	msg.printVerbose("Total Intramolecular Energy (TESTCELL) is %15.9e\n", intraEnergy);
	return totalEnergy + intraEnergy;
}

/*!
 * \brief Test - Return total energy of the system using Molecules
 * \details Calculate the total energy of the system using a loop over Molecules.
 * 
 * This is a serial routine, with each process calculating its own value.
 */
double DUQ::totalEnergyTestMolecules(Configuration& cfg)
{
	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Loop over Molecules
	// We will end up double-counting all Molecule-Molecule interactions, but only single-counting intramolecular terms.
	// So, scale PairPotential contributions to the Grain energy, but leave inter-Grain correction energyies as-is.
	double totalEnergy = 0.0;
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next) totalEnergy += kernel.energy(mol, pairPotentialRangeSquared_, DUQComm::Solo, 0.5, 1.0, 1.0);
	return totalEnergy;
}
