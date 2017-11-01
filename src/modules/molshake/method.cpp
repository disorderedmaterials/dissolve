/*
	*** MolShake Module - Method
	*** src/modules/molshake/method.cpp
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

#include "modules/molshake/molshake.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "base/processpool.h"
#include "base/timer.h"

// Perform set up tasks for module
bool MolShakeModule::setUp(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool MolShakeModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool MolShakeModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform a Molecule shake
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
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Get reference to relevant module data
		GenericList& moduleData = configurationLocal_ ? cfg->moduleData() : duq.processingModuleData();

		// Retrieve control parameters from Configuration
		double cutoffDistance = GenericListHelper<double>::retrieve(moduleData, "CutoffDistance", uniqueName(), keywords_.asDouble("CutoffDistance"));
		if (cutoffDistance < 0.0) cutoffDistance = duq.pairPotentialRange();
		double rotationStepSize = GenericListHelper<double>::retrieve(moduleData, "RotationStepSize", uniqueName(), keywords_.asDouble("RotationStepSize"));
		const int nShakesPerMolecule = GenericListHelper<int>::retrieve(moduleData, "ShakesPerMolecule", uniqueName(), keywords_.asInt("ShakesPerMolecule"));
		const double targetAcceptanceRate = GenericListHelper<double>::retrieve(moduleData, "TargetAcceptanceRate", uniqueName(), keywords_.asDouble("TargetAcceptanceRate"));
		double translationStepSize = GenericListHelper<double>::retrieve(moduleData, "TranslationStepSize", uniqueName(), keywords_.asDouble("TranslationStepSize"));
		const double termScale = 1.0;
		const double rRT = 1.0/(.008314472*cfg->temperature());

		// Print argument/parameter summary
		Messenger::print("MolShake: Cutoff distance is %f\n", cutoffDistance);
		Messenger::print("MolShake: Performing %i shake(s) per Molecule\n", nShakesPerMolecule);
		Messenger::print("MolShake: Translation step is %f Angstroms, rotation step is %f degrees, target acceptance rate is %f.\n", translationStepSize, rotationStepSize, targetAcceptanceRate);

		// Create a local ChangeStore and EnergyKernel
		ChangeStore changeStore(procPool);
		EnergyKernel kernel(procPool, cfg, duq.potentialMap(), cutoffDistance);

		// Initialise the random number buffer
		procPool.initialiseRandomBuffer(ProcessPool::Group);

		// Enter calculation loop until no more Cells are available
		int cellId, shake, n, nbr;
		int nTries = 0, nAccepted = 0;
		bool accept;
		double currentEnergy, newEnergy, delta, totalDelta = 0.0;
		Vec3<double> rDelta;
		Matrix3 transform;
		const Box* box = cfg->box();

		Timer timer;
		procPool.resetAccumulatedTime();
		for (int m = 0; m < cfg->nMolecules(); ++m)
		{
			/*
			* Calculation Begins
			*/

			// Get Molecule pointer
			Molecule* mol = cfg->molecule(m);

			// Set current atom targets in ChangeStore (entire cell contents)
			changeStore.add(mol);

			// Calculate reference energy for Molecule, including intramolecular terms
			currentEnergy = kernel.energy(mol, ProcessPool::OverPoolProcesses);

			// Loop over number of shakes per atom
			for (shake=0; shake<nShakesPerMolecule; ++shake)
			{
				// Create a random translation vector and apply it to the Molecule's centre
				rDelta.set(procPool.randomPlusMinusOne()*translationStepSize, procPool.randomPlusMinusOne()*translationStepSize, procPool.randomPlusMinusOne()*translationStepSize);
				mol->translateCentre(rDelta);

				// Create a random rotation matrix and apply it to the Molecule
				transform.createRotationXY(procPool.randomPlusMinusOne()*180.0, procPool.randomPlusMinusOne()*180.0);
				mol->applyTransform(box, transform);
				
				// Calculate new energy
				newEnergy = kernel.energy(mol, ProcessPool::OverPoolProcesses);
				
				// Trial the transformed atom position
				delta = newEnergy - currentEnergy;
// 				printf("delta = %f\n", delta);
				accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

				if (accept)
				{
// 					Messenger::print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Atom
					changeStore.updateAll();
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revertAll();
				
				++nTries;
			}

			// Store modifications to Atom positions ready for broadcast
			changeStore.storeAndReset();

			/*
			* Calculation End
			*/

			// Distribute coordinate changes to all processes
			changeStore.distributeAndApply(cfg);
			changeStore.reset();
		}
		timer.stop();

		// Collect statistics from process group leaders
// 		if (!procPool.allSum(&nAccepted, 1, ProcessPool::Leaders)) return false;
// 		if (!procPool.allSum(&nTries, 1, ProcessPool::Leaders)) return false;
// 		if (!procPool.allSum(&totalDelta, 1, ProcessPool::Leaders)) return false;
		if (procPool.groupLeader())
		{
			double rate = double(nAccepted)/nTries;

			Messenger::print("MolShake: Overall acceptance rate was %4.2f% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.totalTimeString(), procPool.accumulatedTimeString());
			Messenger::print("MolShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

			// Adjust step size - if nAccepted was zero, just decrease the current stepSize by a constant factor
			translationStepSize *= (nAccepted == 0) ? 0.8 : rate/targetAcceptanceRate;

			// Clamp step size
	// 		if (stepSize > 0.5) stepSize = 0.5;
	// 		else if (stepSize_ > maxTranslationStep_) stepSize_ = maxTranslationStep_;
	// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
		}

		// Store updated parameter values
		if (!procPool.broadcast(&translationStepSize, 1, 0, ProcessPool::Group)) return false;
		GenericListHelper<double>::realise(cfg->moduleData(), "TranslationStepSize", uniqueName(), GenericItem::InRestartFileFlag) = translationStepSize;
		GenericListHelper<double>::realise(cfg->moduleData(), "RotationStepSize", uniqueName(), GenericItem::InRestartFileFlag) = rotationStepSize;
		Messenger::print("MolShake: Updated translation step is %f Angstroms, rotation step is %f degrees.\n", translationStepSize, rotationStepSize);
		
		// Increment configuration changeCount_
		if (nAccepted > 0) cfg->incrementCoordinateIndex();

		// Update total energy
		cfg->registerEnergyChange(totalDelta);
		cfg->accumulateEnergyChange();
	}

	return true;
}

// Execute post-processing stage
bool MolShakeModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return true;
}
