/*
	*** Geometry Optimisation Module - Processing
	*** src/modules/geomopt/process.cpp
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

#include "modules/geomopt/geomopt.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "main/dissolve.h"
#include "base/sysfunc.h"

// Run main processing
bool GeometryOptimisationModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Retrieve Module options
	const int nCycles = keywords_.asInt("NCycles");
	const double tolerance = keywords_.asDouble("Tolerance");
	const double initialStepSize = keywords_.asDouble("StepSize");
	const int nStepSizeResetsAllowed = 5;

	// Print argument/parameter summary
	Messenger::print("Optimise: Maximum number of cycles is %i.\n", nCycles);
	Messenger::print("Optimise: Base convergence tolerance is %e.\n", tolerance);
	Messenger::print("Optimise: Initial step size to be used is %e.\n", initialStepSize);
	Messenger::print("\n");

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

		bool converged = false, lineDone = false;

		// Get the initial energy and forces of the Configuration
		double oldEnergy = EnergyModule::totalEnergy(procPool, cfg, dissolve.potentialMap());
		ForcesModule::totalForces(procPool, cfg, dissolve.potentialMap(), xForce_, yForce_, zForce_);
		double oldRMSForce = rmsForce();

		// Set initial step size - the line minimiser will modify this as we proceed
		double stepSize = initialStepSize;

		Messenger::print("Cycle  %-16s  %-16s  %-16s  %-16s  %-16s\n", "E(total), kJ/mol", "dE, kJ/mol", "RMS(force)", "dRMS", "Step Size");
		Messenger::print(" --    %16.9e  %-16s  %16.9e  %-16s  %16.9e\n", oldEnergy, "------", oldRMSForce, "------", stepSize);

		int nStepSizeResets = 0;
		for (int cycle = 1; cycle <= nCycles; ++cycle)
		{
			// Copy current Configuration coordinates as our reference (they will be modified by lineMinimise())
			setReferenceCoordinates(cfg);

			// Line minimise along the force gradient
			double newEnergy = lineMinimise(procPool, cfg, dissolve.potentialMap(), tolerance*0.01, stepSize);

			// Get new forces and RMS for the adjusted coordinates (now stored in the Configuration) and determine new step size
			ForcesModule::totalForces(procPool, cfg, dissolve.potentialMap(), xForce_, yForce_, zForce_);
			double newRMSForce = rmsForce();

			// Calculate deltas
			double dE = newEnergy - oldEnergy;
			double dF = newRMSForce - oldRMSForce;

			// Print summary
			Messenger::print("%5i  %16.9e  %16.9e  %16.9e  %16.9e  %16.9e\n", cycle, newEnergy, dE, newRMSForce, dF, stepSize);

			// Check convergence
			if ((fabs(dE) < tolerance) || (fabs(dF) < tolerance))
			{
				// Reset the step size and try again?
				if (nStepSizeResets < nStepSizeResetsAllowed)
				{
					++nStepSizeResets;
					stepSize = initialStepSize;
				}
				else
				{
					Messenger::print(" *** Steepest Descent converged at step %i ***", cycle);
					break;
				}
			}

			// Store new energy / forces as current forces
			oldEnergy = newEnergy;
			oldRMSForce = newRMSForce;
		}
	}

	return true;
}

