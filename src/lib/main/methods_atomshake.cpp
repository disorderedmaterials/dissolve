/*
	*** dUQ Methods - AtomShake
	*** src/lib/main/methods_atomshake.cpp
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
 * \brief Perform an Atom shake
 * \details
 * 
 * This is a parallel routine, with processes operating in groups.
 */
CommandReturnValue DUQ::atomShake(Configuration& cfg)
{
	// Control Parameters
	const double termScale = 1.0;
	
	// Get method arguments and parameters from Command
	bool result;
	// -- Number of shakes per Atom
	int nShakesPerAtom = commandArgumentAsInteger("atomshake", "n", result);
	if (!result) return CommandFail;
	// -- Energy cutoff 
	double cutoffSq = commandArgumentAsDouble("atomshake", "cut", result);
	if (!result) return CommandFail;
	if (cutoffSq < 0.0) cutoffSq = pairPotentialRange_;
	// -- Translation step size
	Argument* translationStepParam = commandParameter("atomshake", "maxtrans");
	if (!translationStepParam) return CommandFail;
	double translationStep = translationStepParam->asDouble();
	// -- Target acceptance rate
	Argument* rateParam = commandParameter("atomshake", "rate");
	if (!rateParam) return CommandFail;
	double acceptanceRate = rateParam->asDouble();

	// Print argument/parameter summary
	msg.print("AtomShake: Cutoff distance is %f\n", cutoffSq);
	msg.print("AtomShake: Performing %i shake(s) per Atom\n", nShakesPerAtom);
	msg.print("AtomShake: Translation step is %f Angstroms, target acceptance rate is %f.\n", translationStep, acceptanceRate);
	cutoffSq *= cutoffSq;

	// Initialise the Cell distributor
	const bool willBeModified = TRUE, allowRepeats = FALSE;
	cfg.initialiseCellDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(DUQComm::Group);

	// Enter calculation loop until no more Cells are available
	int cellId, n, shake, m;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double currentEnergy, newEnergy, delta, totalDelta = 0.0;
	Cell* cell;
	Atom* i;
	Grain* grainI;
	Vec3<double> centre, rDelta;

	Timer timer;
	Comm.resetAccumulatedTime();
	while (cellId = cfg.nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distribute(cfg.nAtoms(), cfg.atoms());
			cfg.finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg.box()->cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nNeighbours());

		/*
		 * Calculation Begin
		 */

		for (n=0; n<cell->nGrains(); ++n)
		{
			// Get current Grain and loop over Atoms
			grainI = cell->grain(n);
			nTries += grainI->nAtoms() * nShakesPerAtom;

			// Calculate current reference energy - base it on the current Grain since this is a convenient unit
			currentEnergy = kernel.energy(grainI, cell->neighbours(), cutoffSq, FALSE, DUQComm::Group);
			currentEnergy += kernel.energy(grainI, cell, cutoffSq, FALSE, FALSE, DUQComm::Group);
			// -- Add on all internal/connection terms associated with the Grain
			currentEnergy += kernel.fullIntraEnergy(grainI, termScale);

			for (m=0; m<grainI->nAtoms(); ++m)
			{
				i = grainI->atom(m);

				// Set current Grain as target in ChangeStore
				changeStore.add(grainI);

				// Loop over number of shakes per Grain
				for (shake=0; shake<nShakesPerAtom; ++shake)
				{
					// Create a random translation vector
					rDelta.set(Comm.randomPlusMinusOne()*translationStep, Comm.randomPlusMinusOne()*translationStep, Comm.randomPlusMinusOne()*translationStep);

					// Translate atom and calculate new energy
					i->translateCoordinates(rDelta);
					newEnergy = kernel.energy(grainI, cell->neighbours(), cutoffSq, FALSE, DUQComm::Group);
					newEnergy += kernel.energy(grainI, cell, cutoffSq, FALSE, FALSE, DUQComm::Group);
					newEnergy += kernel.fullIntraEnergy(grainI, termScale);

					// Trial the transformed Grain position (the Master is in charge of this)
					delta = newEnergy - currentEnergy;
					accept = delta < 0 ? TRUE : (Comm.random() < exp(-delta/(.008314472*temperature_)));

					if (accept)
					{
	// 					msg.print("Accepts move with delta %f\n", delta);
						// Accept new (current) position of target Grain
						changeStore.updateAtomsLocal(1, &m);
						currentEnergy = newEnergy;
						totalDelta += delta;
						++nAccepted;
					}
					else changeStore.revert();
				}
			}

			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();
		}

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distribute(cfg.nAtoms(), cfg.atoms());
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

		msg.print("AtomShake: Overall acceptance rate was %4.2f% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());
		msg.print("AtomShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Adjust step size
		translationStep *= rate/acceptanceRate;
// 		if (translationStep_ < 0.05) translationStep_ = 0.05;
// 		else if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
	}

	// Store updated parameter values
	if (!Comm.broadcast(&translationStep, 1, 0, DUQComm::Group)) return CommandCommFail;
	translationStepParam->setValue(translationStep);
	msg.print("AtomShake: Updated translation step is %f Angstroms.\n", translationStep);
	
	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	// Update total energy
	registerEnergyChange(totalDelta);
	accumulateEnergyChange();

	return CommandSuccess;
}
