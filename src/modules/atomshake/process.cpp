/*
	*** AtomShake Module - Processing
	*** src/modules/atomshake/process.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/atomshake/atomshake.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/celldistributor.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "base/processpool.h"
#include "base/timer.h"
#include "templates/genericlisthelper.h"

// Run pre-processing stage
bool AtomShakeModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Run main processing
bool AtomShakeModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform an Atom shake
	 * 
	 * This is a parallel routine, with processes operating in groups.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Get reference to relevant module data
		GenericList& moduleData = configurationLocal_ ? cfg->moduleData() : duq.processingModuleData();

		// Retrieve control parameters from Configuration
		double cutoffDistance = keywords_.asDouble("CutoffDistance");
		if (cutoffDistance < 0.0) cutoffDistance = duq.pairPotentialRange();
		const int nShakesPerAtom = keywords_.asInt("ShakesPerAtom");
		const double targetAcceptanceRate = keywords_.asDouble("TargetAcceptanceRate");
		double stepSize = GenericListHelper<double>::retrieve(moduleData, "StepSize", uniqueName(), keywords_.asDouble("StepSize"));
		const double termScale = 1.0;
		const double rRT = 1.0/(.008314472*cfg->temperature());

		// Print argument/parameter summary
		Messenger::print("AtomShake: Cutoff distance is %f\n", cutoffDistance);
		Messenger::print("AtomShake: Performing %i shake(s) per Atom\n", nShakesPerAtom);
		Messenger::print("AtomShake: Translation step is %f Angstroms, target acceptance rate is %f.\n", stepSize, targetAcceptanceRate);

		ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();

		// Initialise a CellDistributor
		CellArray& cellArray = cfg->cells();
		CellDistributor distributor(cellArray, procPool, strategy, false);

		// Create a local ChangeStore and EnergyKernel
		ChangeStore changeStore(procPool);
		EnergyKernel kernel(procPool, cfg, duq.potentialMap(), cutoffDistance);

		// Initialise the random number buffer so it is suitable for our parallel strategy within the main loop
		procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));

		int cellId, shake, n, nbr;
		int nTries = 0, nAccepted = 0;
		bool accept;
		double currentEnergy, intraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
		Cell* cell;
		Vec3<double> centre, rDelta;
		Atom** cellAtoms;
		bool changesBroadcastRequired;

		Timer timer;
		procPool.resetAccumulatedTime();
		while (cellId = distributor.nextAvailableObject(changesBroadcastRequired), cellId != Distributor::AllComplete)
		{
			// Upkeep - Do we need to broadcast changes before we begin the calculation?
			if (changesBroadcastRequired)
			{
				changeStore.distributeAndApply(cfg);
				changeStore.reset();
			}

			// Check for valid cell
			if (cellId == CellDistributor::NoneAvailable)
			{
				distributor.finishedWithObject();
				continue;
			}

			/*
			 * Calculation Begins
			 */

			cell = cellArray.cell(cellId);
			Messenger::printVerbose("AtomShake: Cell %i now the target on process %s, containing %i Atoms interacting with %i neighbour cells.\n", cellId, procPool.processInfo(), cell->nAtoms(), cell->nTotalCellNeighbours());

			// Set current atom targets in ChangeStore (entire cell contents)
			changeStore.add(cell);

			// Loop over atoms in this cell
			cellAtoms = cell->atoms().objects();
			for (n = 0; n < cell->atoms().nItems(); ++n)
			{
				// Grab Atom pointer
				Atom* i = cellAtoms[n];

				// Calculate reference energy for atom, including intramolecular terms
				currentEnergy = kernel.energy(i, ProcessPool::subDivisionStrategy(strategy), true);
				intraEnergy = kernel.intraEnergy(i) * termScale;

				// Loop over number of shakes per atom
				for (shake=0; shake<nShakesPerAtom; ++shake)
				{
					// Create a random translation vector
					rDelta.set(procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize);

					// Translate atom and calculate new energy
					i->translateCoordinates(rDelta);
					newEnergy = kernel.energy(i, ProcessPool::subDivisionStrategy(strategy), true);
					newIntraEnergy = kernel.intraEnergy(i) * termScale;
					
					// Trial the transformed atom position
					delta = (newEnergy + newIntraEnergy) - (currentEnergy + intraEnergy);
	// 				printf("delta = %f\n", delta);
					accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

					if (accept)
					{
	// 					Messenger::print("Accepts move with delta %f\n", delta);
						// Accept new (current) position of target Atom
						changeStore.updateAtom(n);
						currentEnergy = newEnergy;
						intraEnergy = newIntraEnergy;
						totalDelta += delta;
						++nAccepted;
					}
					else changeStore.revert(n);
					
					++nTries;
				}

			}

			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();

			/*
			* Calculation End
			*/

			// Tell the distributor we are done
			distributor.finishedWithObject();
		}

		// Make sure any remaining changes are broadcast
		changeStore.distributeAndApply(cfg);
		changeStore.reset();

		timer.stop();

		// Collect statistics across all processe
		if (!procPool.allSum(&nAccepted, 1, strategy)) return false;
		if (!procPool.allSum(&nTries, 1, strategy)) return false;
		if (!procPool.allSum(&totalDelta, 1, strategy)) return false;

		double rate = double(nAccepted)/nTries;

		Messenger::print("AtomShake: Overall acceptance rate was %4.2f% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.totalTimeString(), procPool.accumulatedTimeString());
		Messenger::print("AtomShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Adjust step size - if nAccepted was zero, just decrease the current stepSize by a constant factor
		stepSize *= (nAccepted == 0) ? 0.8 : rate/targetAcceptanceRate;

		// Clamp step size
// 		if (stepSize > 0.5) stepSize = 0.5;
// 		else if (stepSize_ > maxTranslationStep_) stepSize_ = maxTranslationStep_;
// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;

		// Store updated parameter values
		GenericListHelper<double>::realise(moduleData, "StepSize", uniqueName(), GenericItem::InRestartFileFlag) = stepSize;
		Messenger::print("AtomShake: Updated translation step is %f Angstroms.\n", stepSize);
		
		// Increase coordinate index in Configuration
		if (nAccepted > 0) cfg->incrementCoordinateIndex();
	}

	return true;
}

// Run post-processing stage
bool AtomShakeModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return true;
}
