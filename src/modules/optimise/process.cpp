/*
	*** Optimise Module - Processing
	*** src/modules/optimise/process.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/optimise/optimise.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "main/dissolve.h"
#include "base/sysfunc.h"

// Run main processing
bool OptimiseModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * This is a XXX routine.
	 * XXX
	 */

	// TODO For options
	int nCycles_ = 10;

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

		// Initialise working arrays for coordinates and forces
		xRef_.initialise(cfg->nAtoms());
		yRef_.initialise(cfg->nAtoms());
		zRef_.initialise(cfg->nAtoms());
		xTemp_.initialise(cfg->nAtoms());
		yTemp_.initialise(cfg->nAtoms());
		zTemp_.initialise(cfg->nAtoms());
		xForce_.initialise(cfg->nAtoms());
		yForce_.initialise(cfg->nAtoms());
		zForce_.initialise(cfg->nAtoms());

		// Get the current energy of the Configuration
		double currentEnergy = EnergyModule::totalEnergy(procPool, cfg, dissolve.potentialMap());

		bool converged = false, lineDone = false;

		double stepSize = 1.0e-4;
		for (int cycle = 0; cycle < nCycles_; ++cycle)
		{
			// Simple method begins here
			double oldEnergy = currentEnergy;
// 			double oldForceRMS = newForceRMS;

// 				// Minimise along gradient vector
// 				if (simple)
// 				{
// 					// Step along gradient (with reducing step size until energy decreases)
// 					nattempts = 0;
// 					do
// 					{
// 						++nattempts;
// 						gradientMove(sourceModel, stepsize);
// 						currentEnergy = sourceModel->totalEnergy(&tempModel_, success);
// 						if (currentEnergy > oldEnergy) stepsize *= 0.5;
// 					} while (currentEnergy > oldEnergy);
// 					// If the very first attempt was successful, increase the stepsize again
// 					if (nattempts == 1) stepsize *= 1.5;
// 				}
// 				else 

			// Copy current Configuration coordinates as our reference (they will be modified by lineMinimise())
			setReferenceCoordinates(cfg);

			// Calculate current forces and corresponding RMS
			ForcesModule::totalForces(procPool, cfg, dissolve.potentialMap(), xForce_, yForce_, zForce_);
	// 		douoble currentRMSForce = sourceModel->rmsForce();
			
			currentEnergy = lineMinimise(procPool, cfg, dissolve.potentialMap(), 0.0001, stepSize);

			printf("ENERGY = %f\n", currentEnergy);
		}

// 			// Print out the step data
// 			if (cycle%5 == 0)
// 			{
// 				Messenger::print("%-5i %12.5e  %12.5e  %12.5e  %12.5e  %12.5e  %12.5e  %12.5e  %12.5e", cycle+1, currentEnergy, currentEnergy-lastPrintedEnergy, newForce, sourceModel->energy.vdw(), sourceModel->energy.electrostatic(), sourceModel->energy.bond(), sourceModel->energy.angle(), sourceModel->energy.torsion());
// 				lastPrintedEnergy = currentEnergy;
// 			}

// 			if (lineDone || converged) break;
// 		}
	}

	return false;
}

