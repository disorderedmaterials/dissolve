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
#include "classes/species.h"
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
	EnergyKernel kernel(cfg, potentialMap_);

	double energy = 0.0;
	int start, stride;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Main loop over molecules
	for (int m=start; m<cfg.nMolecules(); m += stride)
	{
		Molecule* mol = cfg.molecule(m);

		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next) energy += kernel.energy(mol, b);

		// Angles
		for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next) energy += kernel.energy(mol, a);
	}

	msg.printVerbose("Intramolecular Energy (Local) is %15.9e\n", energy);
	
	// Sum energy and print
	Comm.allSum(&energy, 1);
	msg.printVerbose("Intramolecular Energy (World) is %15.9e\n", energy);

	return energy;
}

/*!
 * \brief Return atom energy of the system
 * \details Calculates the total atom energy of the system, i.e. the energy contributions from PairPotential
 * interactions between individual atoms.
 * 
 * This is a parallel routine, with processes operating as process groups.
 */
double DUQ::interatomicEnergy(Configuration& cfg)
{
	// Initialise the Cell distributor
	const bool willBeModified = false, allowRepeats = false;
	cfg.initialiseCellDistribution();

	// Create an EnergyKernel
	EnergyKernel kernel(cfg, potentialMap_);

	int cellId, n, m, start, stride;
	Cell* cell, *otherCell;
	double totalEnergy = 0.0;
	
	// TEST Invalidate all cell atom lists
	for (int n=0; n<cfg.nCells(); ++n)
	{
		cfg.cell(n)->atoms().invalidateLists();
		cfg.cell(n)->atomNeighbours().invalidateLists();
		cfg.cell(n)->mimAtomNeighbours().invalidateLists();
	}

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::Group);
	stride = Comm.interleavedLoopStride(DUQComm::Group);

	for (cellId = start; cellId<cfg.nCells(); cellId += stride)
	{
		cell = cfg.cell(cellId);

		/*
		 * Calculation Begins
		 */

		// This cell with itself
// 		printf("Cell index %i:\n", cell->index());
// 		for (n=0; n<cell->maxAtoms(); ++n) if (cell->atom(n)->index() != Atom::UnusedAtom) printf("   %3i  %s  %3i  %3i %f  %f  %f\n", n, typeIndex_[cell->atom(n)->atomTypeIndex()]->name(), cell->atom(n)->index(), cell->atom(n)->moleculeAtomIndex(), cell->atom(n)->r().x, cell->atom(n)->r().y, cell->atom(n)->r().z);
		totalEnergy += kernel.energy(cell, cell, false, true, DUQComm::Solo);

		// Interatomic interactions between atoms in this cell and its neighbours
// 		for (RefListItem<Cell,bool>* ri = cell->neighbours().first(); ri != NULL; ri = ri->next)
// 		{
// 			// Perform i >= j exclusion here by comparing cell indices rather than atom indices, since for the i-j and j-i pairs will both be trialled at some point
// 			if (cell->index() >= ri->item->index()) continue;
// 			totalEnergy += kernel.energy(cell, ri->item, ri->data, false, DUQComm::Solo);
// 		}
		totalEnergy += kernel.energy(cell, true, DUQComm::Solo);

		/*
		 * Calculation End
		 */
	}
	msg.printVerbose("Atom Energy (Local) is %15.9e\n", totalEnergy);

	// Sum energy and print
	Comm.allSum(&totalEnergy, 1);
	msg.printVerbose("Atom Energy (World) is %15.9e\n", totalEnergy);

	return totalEnergy;
}

/*!
 * \brief Return Grain energy of the system
 * \details Calculates the total Grain energy of the system, i.e. the energy contributions from PairPotential
 * interactions between Grains.
 * 
 * This is a parallel routine, with processes operating as process groups.
 */
double DUQ::intergrainEnergy(Configuration& cfg)
{
	// Initialise the Cell distributor
	const bool willBeModified = false, allowRepeats = false;
	cfg.initialiseCellDistribution();

	// Create an EnergyKernel
	EnergyKernel kernel(cfg, potentialMap_);

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
		cell = cfg.cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nTotalCellNeighbours());

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
				totalEnergy += kernel.energy(grainI, grainJ, true, false);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			totalEnergy += kernel.energy(grainI, cell->atomNeighbours(), false, true, DUQComm::Solo);
			totalEnergy += kernel.energy(grainI, cell->mimAtomNeighbours(), true, true, DUQComm::Solo);
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

	double atomEnergy, intraEnergy;

	// Calculate Grain energy
	Timer grainTimer;
	atomEnergy = interatomicEnergy(cfg);
	grainTimer.stop();

	// Calculate intramolecular and interGrain correction energy
	Timer intraTimer;
	intraEnergy = intramolecularEnergy(cfg);
	intraTimer.stop();

	msg.print("Time to do atom energy was %s, intramolecular energy was %s.\n", grainTimer.timeString(), intraTimer.timeString());
	msg.print("Total Energy (World) is %15.9e (%15.9e Grain + %15.9e Intramolecular)\n", atomEnergy + intraEnergy, atomEnergy, intraEnergy);

	return atomEnergy + intraEnergy;
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
	Atom* i, *j, *k;
	Vec3<double> vecji, vecjk;

	// Main loop over molecules
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next)
	{
		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			// Grab pointers to atoms involved in bond
			i = mol->atom(b->indexI());
			j = mol->atom(b->indexJ());

			distanceSq = cfg.box()->minimumDistanceSquared(i, j);
			intraEnergy += b->energy(sqrt(distanceSq));
		}

		// Angles
		for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
		{
			// Grab pointers to atoms involved in angle
			i = mol->atom(a->indexI());
			j = mol->atom(a->indexJ());
			k = mol->atom(a->indexK());

			// Get vectors 'j-i' and 'j-k'
			vecji = cfg.box()->minimumVector(j, i);
			vecjk = cfg.box()->minimumVector(j, k);
			
			// Calculate angle
			vecji.normalise();
			vecjk.normalise();
			angle = Box::angle(vecji, vecjk);

			// Determine Angle energy
			intraEnergy += a->energy(angle);
		}
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
	EnergyKernel kernel(cfg, potentialMap_);

	/*
	 * Calculation Begins
	 */

	double atomEnergy = 0.0, intraEnergy = 0.0;
	Molecule* molN, *molM;
	double scale;
	for (int n=0; n<cfg.nMolecules(); ++n)
	{
		molN = cfg.molecule(n);
		
		// Molecule-molecule energy
		for (int i = 0; i<molN->nAtoms()-1; ++i)
		{
			for (int j = i+1; j<molN->nAtoms(); ++j)
			{
				// Get intramolecular scaling of atom pair
				scale = molN->species()->scaling(i, j);
				if (scale < 1.0e-3) continue;
				atomEnergy += kernel.energy(molN->atom(i), molN->atom(j), true) * scale;
			}
		}

		for (int m=n+1; m<cfg.nMolecules(); ++m)
		{
			molM = cfg.molecule(m);

			// Double loop over atoms
			for (int i = 0; i<molN->nAtoms(); ++i)
			{
				for (int j = 0; j<molM->nAtoms(); ++j) atomEnergy += kernel.energy(molN->atom(i), molM->atom(j), true);
			}
		}
	}

	// Calculate intramolecular energy
	intraEnergy = intramolecularEnergyTest(cfg);

	/*
	 * Calculation End
	 */
	
	msg.print("Total Atom Energy (TEST) is %15.9e\n", atomEnergy);
	msg.print("Total Intramolecular Energy (TEST) is %15.9e\n", intraEnergy);
	msg.print("Total Energy (TEST) is %15.9e\n", atomEnergy + intraEnergy);
	return atomEnergy + intraEnergy;
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
	EnergyKernel kernel(cfg, potentialMap_);

	int cellId, n, m;
	Cell* cell;
	Grain* grainI, *grainJ;
	double totalEnergy = 0.0;
	for (cellId=0; cellId<cfg.nCells(); ++cellId)
	{
		cell = cfg.cell(cellId);

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
				
				totalEnergy += kernel.energy(grainI, grainJ, false, false);
			}
			
			// Inter-Grain interactions between this Grain and those in Cell neighbours
			totalEnergy += kernel.energy(grainI, cell->atomNeighbours(), false, true);
			totalEnergy += kernel.energy(grainI, cell->mimAtomNeighbours(), true, true);
		}

		/*
		 * Calculation End
		 */
	}

	// Calculate intramolecular energy and correction to inter-Grain energy
	double intraEnergy = intramolecularEnergyTest(cfg);

	msg.print("Total Grain Energy (TESTCELL) is %15.9e\n", totalEnergy);
	msg.print("Total Intramolecular Energy (TESTCELL) is %15.9e\n", intraEnergy);
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
	EnergyKernel kernel(cfg, potentialMap_);

	// Loop over Molecules
	// We will end up double-counting all Molecule-Molecule interactions, but only single-counting intramolecular terms.
	// So, scale PairPotential contributions to the Grain energy, but leave inter-Grain correction energyies as-is.
	double totalEnergy = 0.0;
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next) totalEnergy += kernel.energy(mol, DUQComm::Solo, 0.5, 1.0, 1.0);
	return totalEnergy;
}
