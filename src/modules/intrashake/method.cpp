/*
	*** IntraShake Module - Method
	*** src/modules/intrashake/method.cpp
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

#include "modules/intrashake/intrashake.h"
#include "main/duq.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Perform set up tasks for module
bool IntraShakeModule::setUp(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool IntraShakeModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool IntraShakeModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform intramolecular shakes, modifying individual bound terms on molecules.
	 * 
	 * This is a parallel routine, with processes operating as one pool
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
		bool adjustAngles = GenericListHelper<bool>::retrieve(moduleData, "AdjustAngles", uniqueName(), keywords_.asBool("AdjustAngles"));
		bool adjustBonds = GenericListHelper<bool>::retrieve(moduleData, "AdjustBonds", uniqueName(), keywords_.asBool("AdjustBonds"));
		bool adjustTorsions = GenericListHelper<bool>::retrieve(moduleData, "AdjustTorsions", uniqueName(), keywords_.asBool("AdjustTorsions"));
		double angleStepSize = GenericListHelper<double>::retrieve(moduleData, "AngleStepSize", uniqueName(), keywords_.asDouble("AngleStepSize"));
		double bondStepSize = GenericListHelper<double>::retrieve(moduleData, "BondStepSize", uniqueName(), keywords_.asDouble("BondStepSize"));
		double cutoffDistance = GenericListHelper<double>::retrieve(moduleData, "CutoffDistance", uniqueName(), keywords_.asDouble("CutoffDistance"));
		if (cutoffDistance < 0.0) cutoffDistance = duq.pairPotentialRange();
		const int nShakesPerTerm = GenericListHelper<int>::retrieve(moduleData, "ShakesPerTerm", uniqueName(), keywords_.asInt("ShakesPerTerm"));
		const double targetAcceptanceRate = GenericListHelper<double>::retrieve(moduleData, "TargetAcceptanceRate", uniqueName(), keywords_.asDouble("TargetAcceptanceRate"));
		double torsionStepSize = GenericListHelper<double>::retrieve(moduleData, "TorsionStepSize", uniqueName(), keywords_.asDouble("TorsionStepSize"));
		const double rRT = 1.0/(.008314472*cfg->temperature());

		// Print argument/parameter summary
		Messenger::print("IntraShake: Cutoff distance is %f\n", cutoffDistance);
		Messenger::print("IntraShake: Performing %i shake(s) per term\n", nShakesPerTerm);
		Messenger::print("IntraShake: Distance step size for bond adjustments is %f Angstroms.\n", bondStepSize);
		Messenger::print("IntraShake: Angle step size for angle adjustments is %f degrees.\n", angleStepSize);
		Messenger::print("IntraShake: Rotation step size for torsion adjustments is %f degrees.\n", torsionStepSize);
		Messenger::print("IntraShake: Target acceptance rate is %f.\n", targetAcceptanceRate);

		// Create a local ChangeStore and EnergyKernel
		ChangeStore changeStore(procPool);
		EnergyKernel kernel(procPool, cfg, duq.potentialMap(), cutoffDistance);

		// Initialise the random number buffer
		procPool.initialiseRandomBuffer(ProcessPool::Pool);

		int shake, nBondAttempts = 0, nAngleAttempts = 0, nTorsionAttempts = 0, nBondAccepted = 0, nAngleAccepted = 0, nTorsionAccepted = 0;
		int terminus;
		bool accept;
		double ppEnergy, newPPEnergy, intraEnergy, newIntraEnergy, delta, totalDelta = 0.0;
		Vec3<double> vji, vjk, v;
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
			ppEnergy = kernel.energy(mol, ProcessPool::OverPoolProcesses);
			intraEnergy = kernel.intraEnergy(mol);

			// Loop over defined Bonds
			if (adjustBonds) for (int n=0; n<mol->nBonds(); ++n)
			{
				// Grab Bond pointer
				Bond* b = mol->bond(n);

				// Select random terminus
				terminus = procPool.random() > 0.5 ? 1 : 0;

				// Loop over number of shakes per term
				for (shake=0; shake<nShakesPerTerm; ++shake)
				{
					// Get translation vector, normalise, and apply random delta
					vji = box->minimumVector(b->i(), b->j());
					vji.normalise();
					vji *= procPool.randomPlusMinusOne() * bondStepSize;

					// Adjust the Atoms attached to the selected terminus
					printf("NATTACHED %i %i\n", b->nAttached(0), b->nAttached(1));
					mol->translate(vji, b->nAttached(terminus), b->attached(terminus));

					// Calculate new energy
					newPPEnergy = kernel.energy(mol, ProcessPool::OverPoolProcesses);
					newIntraEnergy = kernel.intraEnergy(mol);
					
					// Trial the transformed Molecule
					delta = (newPPEnergy + newIntraEnergy) - (ppEnergy + intraEnergy);
					accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

					// Accept new (current) positions of the Molecule's Atoms?
					if (accept)
					{
						changeStore.updateAll();
						ppEnergy = newPPEnergy;
						intraEnergy = newIntraEnergy;
						totalDelta += delta;
					}
					else changeStore.revertAll();

					++nBondAttempts;
				}
			}

			// Loop over defined Bonds
			if (adjustAngles) for (int n=0; n<mol->nAngles(); ++n)
			{
				// Grab Angle pointer
				Angle* a = mol->angle(n);

				// Select random terminus
				terminus = procPool.random() > 0.5 ? 1 : 0;

				// Loop over number of shakes per term
				for (shake=0; shake<nShakesPerTerm; ++shake)
				{
					// Get bond vectors and calculate cross product to get rotation axis
					vji = box->minimumVector(a->j(), a->i());
					vjk = box->minimumVector(a->k(), a->k());
					v = vji * vjk;

					// Create suitable transformation matrix
					transform.createRotationAxis(v.x, v.y, v.z, procPool.randomPlusMinusOne()*angleStepSize, true);

					// Adjust the Atoms attached to the selected terminus
					mol->transform(transform, a->j()->r(), a->nAttached(terminus), a->attached(terminus));

					// Calculate new energy
					newPPEnergy = kernel.energy(mol, ProcessPool::OverPoolProcesses);
					newIntraEnergy = kernel.intraEnergy(mol);
					
					// Trial the transformed Molecule
					delta = (newPPEnergy + newIntraEnergy) - (ppEnergy + intraEnergy);
					accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

					// Accept new (current) positions of the Molecule's Atoms?
					if (accept)
					{
						changeStore.updateAll();
						ppEnergy = newPPEnergy;
						intraEnergy = newIntraEnergy;
						totalDelta += delta;
					}
					else changeStore.revertAll();

					++nBondAttempts;
				}
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

		Messenger::print("IntraShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);
		Messenger::print("IntraShake: Total number of attempted moves was %i (%s work, %s comms)\n", nBondAttempts+nAngleAttempts+nTorsionAttempts, timer.totalTimeString(), procPool.accumulatedTimeString());

		// Calculate and report acceptance rates and adjust step sizes - if no moves were accepted, just decrease the current stepSize by a constant factor
		if (adjustBonds)
		{
			double bondRate = double(nBondAccepted) / nBondAttempts;
			Messenger::print("IntraShake: Overall bond shake acceptance rate was %4.2f% (%i of %i attempted moves)\n", 100.0*bondRate, nBondAccepted, nBondAttempts);

			bondStepSize *= (nBondAccepted == 0) ? 0.8 : bondRate/targetAcceptanceRate;
			Messenger::print("IntraShake: Updated distance step size for bond adjustments is %f Angstroms.\n", bondStepSize); 
			GenericListHelper<double>::realise(cfg->moduleData(), "BondStepSize", uniqueName(), GenericItem::InRestartFileFlag) = bondStepSize;
		}

		if (adjustAngles)
		{
			double angleRate = double(nAngleAccepted) / nAngleAttempts;
			Messenger::print("IntraShake: Overall angle shake acceptance rate was %4.2f% (%i of %i attempted moves)\n", 100.0*angleRate, nAngleAccepted, nAngleAttempts);

			angleStepSize *= (nAngleAccepted == 0) ? 0.8 : angleRate/targetAcceptanceRate;
			Messenger::print("IntraShake: Updated rotation step size for angle adjustments is %f degrees.\n", angleStepSize); 
			GenericListHelper<double>::realise(cfg->moduleData(), "AngleStepSize", uniqueName(), GenericItem::InRestartFileFlag) = angleStepSize;
		}

		if (adjustTorsions)
		{
			double torsionRate = double(nTorsionAccepted) / nTorsionAttempts;
			Messenger::print("IntraShake: Overall torsion shake acceptance rate was %4.2f% (%i of %i attempted moves)\n", 100.0*torsionRate, nTorsionAccepted, nTorsionAttempts);

			torsionStepSize *= (nTorsionAccepted == 0) ? 0.8 : torsionRate/targetAcceptanceRate;
			Messenger::print("IntraShake: Updated rotation step size for torsion adjustments is %f degrees.\n", torsionStepSize); 
			GenericListHelper<double>::realise(cfg->moduleData(), "TorsionStepSize", uniqueName(), GenericItem::InRestartFileFlag) = torsionStepSize;
		}

		// Increment configuration changeCount_
		if ((nBondAccepted > 0) || (nAngleAccepted > 0) || (nTorsionAccepted > 0)) cfg->incrementCoordinateIndex();

		// Update total energy
		cfg->registerEnergyChange(totalDelta);
		cfg->accumulateEnergyChange();
	}

	return false;
}

// Execute post-processing stage
bool IntraShakeModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
