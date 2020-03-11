/*
	*** AtomShake Module - Processing
	*** src/modules/atomshake/process.cpp
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

#include "modules/atomshake/atomshake.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "classes/regionaldistributor.h"
#include "base/processpool.h"
#include "base/timer.h"
#include "genericitems/listhelper.h"

// Run main processing
bool AtomShakeModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Perform an Atom shake
	 * 
	 * This is a parallel routine, with processes operating in groups.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0) return Messenger::error("No configuration targets set for module '%s'.\n", uniqueName());

	// Loop over target Configurations
	RefListIterator<Configuration> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Retrieve control parameters from Configuration
		double cutoffDistance = keywords_.asDouble("CutoffDistance");
		if (cutoffDistance < 0.0) cutoffDistance = dissolve.pairPotentialRange();
		const int nShakesPerAtom = keywords_.asInt("ShakesPerAtom");
		const double targetAcceptanceRate = keywords_.asDouble("TargetAcceptanceRate");
		double& stepSize = keywords_.retrieve<double>("StepSize");
		const double stepSizeMax = keywords_.asDouble("StepSizeMax");
		const double stepSizeMin = keywords_.asDouble("StepSizeMin");
		const double termScale = 1.0;
		const double rRT = 1.0/(.008314472*cfg->temperature());

		// Print argument/parameter summary
		Messenger::print("AtomShake: Cutoff distance is %f\n", cutoffDistance);
		Messenger::print("AtomShake: Performing %i shake(s) per Atom\n", nShakesPerAtom);
		Messenger::print("AtomShake: Step size for adjustments is %f Angstroms (allowed range is %f <= delta <= %f).\n", stepSize, stepSizeMin, stepSizeMax);
		Messenger::print("AtomShake: Target acceptance rate is %f.\n", targetAcceptanceRate);
		Messenger::print("\n");

		ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();

		// Create a Molecule distributor
		DynamicArray<Molecule>& moleculeArray = cfg->molecules();
		RegionalDistributor distributor(moleculeArray, cfg->cells(), procPool, strategy);

		// Create a local ChangeStore and EnergyKernel
		ChangeStore changeStore(procPool);
		EnergyKernel kernel(procPool, cfg, dissolve.potentialMap(), cutoffDistance);

		// Initialise the random number buffer so it is suitable for our parallel strategy within the main loop
		procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));

		int shake, n, m;
		int nAttempts = 0, nAccepted = 0;
		bool accept;
		double currentEnergy, currentIntraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
		Vec3<double> rDelta;

		int molId;

		Timer timer;
		procPool.resetAccumulatedTime();
		while (distributor.cycle())
		{
			// Get next set of Molecule targets from the distributor
			Array<int> targetMolecules = distributor.assignedMolecules();

			// Switch parallel strategy if necessary
			if (distributor.currentStrategy() != strategy)
			{
				// Set the new strategy
				strategy = distributor.currentStrategy();

				// Re-initialise the random buffer
				procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));
			}

			// Loop over target Molecules
			for (m = 0; m<targetMolecules.nItems(); ++m)
			{
				/*
				 * Calculation Begins
				 */

				// Get Molecule index and pointer
				molId = targetMolecules[m];
				Molecule* mol = cfg->molecule(molId);

				// Set current Atom targets in ChangeStore (whole Molecule)
				changeStore.add(mol);

				n = 0;
				// Loop over atoms in the Molecule
				for (auto* i : mol->atoms())
				{
					// Calculate reference energy for the Atom
					currentEnergy = kernel.energy(i, ProcessPool::subDivisionStrategy(strategy), true);
					currentIntraEnergy = kernel.intramolecularEnergy(mol, i) * termScale;

					// Loop over number of shakes per Atom
					for (shake=0; shake<nShakesPerAtom; ++shake)
					{
						// Create a random translation vector
						rDelta.set(procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize);

						// Translate Atom and update its Cell position
						i->translateCoordinates(rDelta);
						cfg->updateCellLocation(i);

						// Calculate new energy
						newEnergy = kernel.energy(i, ProcessPool::subDivisionStrategy(strategy), true);
						newIntraEnergy = kernel.intramolecularEnergy(mol, i) * termScale;
						
						// Trial the transformed Atom position
						delta = (newEnergy + newIntraEnergy) - (currentEnergy + currentIntraEnergy);
						accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

						if (accept)
						{
							// Accept new (current) position of target Atom
							changeStore.updateAtom(n);
							currentEnergy = newEnergy;
						}
						else changeStore.revert(n);

						// Increase attempt counters
						// The strategy in force at any one time may vary, so use the distributor's helper functions.
						if (distributor.collectStatistics())
						{
							if (accept)
							{
								totalDelta += delta;
								++nAccepted;
							}
							++nAttempts;
						}
						++n;
					}

				}

				// Store modifications to Atom positions ready for broadcast later
				changeStore.storeAndReset();

				/*
				 * Calculation End
				 */
			}

			// Now all target Molecules have been processes, broadcast the changes made
			changeStore.distributeAndApply(cfg);
			changeStore.reset();
		}

		// Collect statistics across all processe
		if (!procPool.allSum(&nAccepted, 1, strategy)) return false;
		if (!procPool.allSum(&nAttempts, 1, strategy)) return false;
		if (!procPool.allSum(&totalDelta, 1, strategy)) return false;

		timer.stop();

		Messenger::print("Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Calculate and print acceptance rate
		double rate = double(nAccepted)/nAttempts;
		Messenger::print("Total number of attempted moves was %i (%s work, %s comms)\n", nAttempts, timer.totalTimeString(), procPool.accumulatedTimeString());

		Messenger::print("Overall acceptance rate was %4.2f% (%i of %i attempted moves)\n", 100.0*rate, nAccepted, nAttempts);

		// Update and set translation step size
		stepSize *= (nAccepted == 0) ? 0.8 : rate/targetAcceptanceRate;
		if (stepSize < stepSizeMin) stepSize = stepSizeMin;
		else if (stepSize > stepSizeMax) stepSize = stepSizeMax;

		Messenger::print("Updated step size is %f Angstroms.\n", stepSize); 

		// Increase contents version in Configuration
		if (nAccepted > 0) cfg->incrementContentsVersion();
	}

	return true;
}

