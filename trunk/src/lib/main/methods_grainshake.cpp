/*
	*** dUQ Methods - GrainShake
	*** src/lib/main/methods_grainshake.cpp
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
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "base/comms.h"
#include "base/timer.h"
#include "math/matrix4.h"

/*!
 * \brief Perform a Grain shake
 * \details
 * 
 * This is a parallel routine, operating in process groups.
 */
CommandReturnValue DUQ::grainShake(Configuration& cfg)
{
	// Control Parameters
	const double termScale = 1.0;

	// Get method arguments and parameters from Command
	bool result;
	// -- Number of shakes per Grain
	int nShakesPerGrain = commandArgumentAsInteger("grainshake", "n", result);
	if (!result) return CommandFail;
	// -- Energy cutoff 
	double cutoffSq = commandArgumentAsDouble("grainshake", "cut", result);
	if (!result) return CommandFail;
	if (cutoffSq < 0.0) cutoffSq = pairPotentialRange_;
	// -- Rotation step size
	Argument* rotationStepParam = commandParameter("grainshake", "maxrot");
	if (!rotationStepParam) return CommandFail;
	double rotationStep = rotationStepParam->asDouble();
	// -- Translation step size
	Argument* translationStepParam = commandParameter("grainshake", "maxtrans");
	if (!translationStepParam) return CommandFail;
	double translationStep = translationStepParam->asDouble();
	// -- Target acceptance rate
	Argument* rateParam = commandParameter("grainshake", "rate");
	if (!rateParam) return CommandFail;
	double acceptanceRate = rateParam->asDouble();

	// Print argument/parameter summary
	msg.print("GrainShake: Cutoff distance is %f\n", cutoffSq);
	msg.print("GrainShake: Performing %i shake(s) per Grain\n", nShakesPerGrain);
	msg.print("GrainShake: Translation step is %f Angstroms, rotation step is %f degrees, target acceptance rate is %f.\n", translationStep, rotationStep, acceptanceRate);
	cutoffSq *= cutoffSq;

	// Initialise the Cell distributor
	const bool willBeModified = true, allowRepeats = false;
	cfg.initialiseCellDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg, potentialMap_);

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(DUQComm::Group);

	// Enter calculation loop until no more Cells are available
	int cellId, n, shake, m;
	Matrix4 transform;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Cell* cell;
	Atom* i;
	Grain* grainI;
	Vec3<double> centre;

	Timer timer;
	Comm.resetAccumulatedTime();
	while (cellId = cfg.nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distribute(cfg);
			cfg.finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg.cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nNeighbours());

		/*
		 * Calculation Begin
		 */
		
		nTries += cell->nGrains() * nShakesPerGrain;
		for (n=0; n<cell->nGrains(); ++n)
		{
			// Get current Grain and  
			grainI = cell->grain(n);
			currentEnergy = kernel.energy(grainI, cell->neighbours(), cutoffSq, false, DUQComm::Group);
			currentEnergy += kernel.energy(grainI, cell, cutoffSq, false, false, DUQComm::Group);
			currentEnergy += kernel.fullIntraEnergy(grainI, termScale);

			// Set current Grain as target in ChangeStore
			changeStore.add(grainI);

			// Loop over number of shakes per Grain
			for (shake=0; shake<nShakesPerGrain; ++shake)
			{
				// Create a random transformation matrix
				transform.createRotationXY(Comm.randomPlusMinusOne()*rotationStep, Comm.randomPlusMinusOne()*rotationStep);
				transform.setTranslation(Comm.randomPlusMinusOne()*translationStep, Comm.randomPlusMinusOne()*translationStep, Comm.randomPlusMinusOne()*translationStep);
				
				// Generate new Atom coordinates - must store the original Grain centre, since it will change as soon
				// as an Atom coordinate gets modified.
				centre = grainI->centre();
				for (m=0; m<grainI->nAtoms(); ++m)
				{
					i = grainI->atom(m);
					i->setCoordinates(transform * (i->r() - centre) + centre);
				}

				// Calculate new energy
// 				newEnergy = kernel.energy(grainI, cell->neighbours(), cutoffSq, false, DUQComm::Group);
				newEnergy = kernel.energy(grainI, cell->neighbours(), cutoffSq, false, DUQComm::Group);
				newEnergy += kernel.energy(grainI, cell, cutoffSq, false, false, DUQComm::Group);
				newEnergy += kernel.fullIntraEnergy(grainI, termScale);

				// Trial the transformed Grain position
				delta = newEnergy - currentEnergy;
				accept = delta < 0 ? true : (Comm.random() < exp(-delta/(.008314472*temperature_)));

				if (accept)
				{
// 					msg.print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Grain
					changeStore.updateAll();
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revertAll();
			}
			
			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();
		}

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distribute(cfg);
		changeStore.reset();

		// Must unlock the Cell when we are done with it!
		cfg.finishedWithCell(willBeModified, cellId);
	}
	timer.stop();

	// Grains have moved, so refold and update locations
	updateGrains(cfg);

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&totalDelta, 1, DUQComm::Leaders)) return CommandCommFail;
	if (Comm.processGroupLeader())
	{
		double rate = double(nAccepted)/nTries;

		msg.print("GrainShake: Overall acceptance rate was %4.2f (%i of %i attempted moves) (%s work, %s comms)\n", rate, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());
		msg.print("GrainShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Adjust step size
		translationStep *= rate/acceptanceRate;
		rotationStep *= rate/acceptanceRate;

// 		if (translationStep < 0.05) translationStep = 0.05;
// 		else if (translationStep > maxTranslationStep_) translationStep = maxTranslationStep_;
// 		if (rotationStep < 3.0) rotationStep = 3.0;
	}

	// Store updated parameter values
	if (!Comm.broadcast(&translationStep, 1, 0, DUQComm::Group)) return CommandCommFail;
	if (!Comm.broadcast(&rotationStep, 1, 0, DUQComm::Group)) return CommandCommFail;
	rotationStepParam->setValue(rotationStep);
	translationStepParam->setValue(translationStep);
	msg.print("GrainShake: Updated translation step is %f Angstroms, rotation step is %f degrees.\n", translationStep, rotationStep);

	// Update total energy
	registerEnergyChange(totalDelta);
	accumulateEnergyChange();

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	return CommandSuccess;
}
