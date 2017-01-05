/*
	*** Energy Module
	*** src/modules/energy.cpp
	Copyright T. Youngs 2012-2016

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

#include "modules/energy.h"
#include "main/duq.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Static Members
List<Module> Energy::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
Energy::Energy() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s_%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	options_.add("Test", false);
	options_.add("Save", true);
	options_.add("StabilityWindow", 10);
	options_.add("StabilityThreshold", 0.01);
}

// Destructor
Energy::~Energy()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& Energy::instances()
{
	return instances_;
}

// Create instance of this module
Module* Energy::createInstance()
{
	return new Energy;
}

/*
 * Definition
 */

// Return name of module
const char* Energy::name()
{
	return "Energy";
}

// Return brief description of module
const char* Energy::brief()
{
	return "Calculate the total energy of a Configuration";
}

// Return instance type for module
Module::InstanceType Energy::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool Energy::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool Energy::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool Energy::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* Energy::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool Energy::setupDependentModule(Module* depMod)
{
	return true;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int Energy::nTargetableConfigurations()
{
	return -1;
}

// Return the maximum number of Samples the Module can target (or -1 for any number)
int Energy::nTargetableSamples()
{
	return 0;
}

/*
 * Method
 */

// Perform setup tasks for module
bool Energy::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool Energy::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool Energy::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	* Calculate Energy for the target Configuration(s)
	* 
	* This is a parallel routine, with processes operating in groups, unless in TEST mode.
	 */

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Retrieve control parameters from Configuration
		const bool testMode = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Test", uniqueName(), options_.valueAsBool("Test"));
		const bool saveData = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Save", uniqueName(), options_.valueAsBool("Save"));
		const int stabilityWindow = GenericListHelper<int>::retrieve(cfg->moduleData(), "StabilityWindow", uniqueName(), options_.valueAsInt("StabilityWindow"));
		const double stabilityThreshold = GenericListHelper<double>::retrieve(cfg->moduleData(), "StabilityThreshold", uniqueName(), options_.valueAsDouble("StabilityThreshold"));

		double interEnergy = 0.0, intraEnergy = 0.0;

		// Calculate the total energy
		if (testMode)
		{
			/*
			* Calculate the total energy of the system using a basic loop, with each
			* process calculating its own value.
			* 
			* This is a serial routine, with all processes independently calculating their own value.
			 */

			Messenger::print("Energy: Calculating energy for Configuration '%s' in serial test mode...\n", cfg->name());

			/*
			* Calculation Begins
			 */

			const PotentialMap& potentialMap = duq.potentialMap();

			double distanceSq, angle;
			Atom* i, *j, *k;
			Vec3<double> vecji, vecjk;
			Molecule* molN, *molM;
			const Box* box = cfg->box();
			double scale;

			Timer timer;
			// Calculate interatomic and intramlecular energy in a loop over defined Molecules
			for (int n=0; n<cfg->nMolecules(); ++n)
			{
				molN = cfg->molecule(n);

				// Molecule-molecule energy
				for (int ii = 0; ii <molN->nAtoms()-1; ++ii)
				{
// 					Messenger::print("Atom %i r = %f %f %f\n", ii, molN->atom(ii)->r().x, molN->atom(ii)->r().y, molN->atom(ii)->r().z);
					for (int jj = ii+1; jj <molN->nAtoms(); ++jj)
					{
						// Get intramolecular scaling of atom pair
						scale = molN->species()->scaling(ii, jj);
						if (scale < 1.0e-3) continue;

						interEnergy += potentialMap.energy(molN->atom(ii)->globalTypeIndex(), molN->atom(jj)->globalTypeIndex(), box->minimumDistanceSquared(molN->atom(ii), molN->atom(jj)));
					}
				}

				for (int m=n+1; m<cfg->nMolecules(); ++m)
				{
					molM = cfg->molecule(m);

					// Double loop over atoms
					for (int ii = 0; ii <molN->nAtoms(); ++ii)
					{
						for (int jj = 0; jj <molM->nAtoms(); ++jj) interEnergy += potentialMap.energy(molN->atom(ii)->globalTypeIndex(), molM->atom(jj)->globalTypeIndex(), box->minimumDistanceSquared(molN->atom(ii), molM->atom(jj)));

					}
				}

				// Bond energy
				Species* sp = molN->species();
				for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
				{
					// Grab pointers to atoms involved in bond
					i = molN->atom(b->indexI());
					j = molN->atom(b->indexJ());

					distanceSq = cfg->box()->minimumDistanceSquared(i, j);
					intraEnergy += b->energy(sqrt(distanceSq));
				}

				// Angles
				for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
				{
					// Grab pointers to atoms involved in angle
					i = molN->atom(a->indexI());
					j = molN->atom(a->indexJ());
					k = molN->atom(a->indexK());

					// Get vectors 'j-i' and 'j-k'
					vecji = cfg->box()->minimumVector(j, i);
					vecjk = cfg->box()->minimumVector(j, k);
					
					// Calculate angle
					vecji.normalise();
					vecjk.normalise();
					angle = Box::angle(vecji, vecjk);

					// Determine Angle energy
					intraEnergy += a->energy(angle);
				}
			}
			timer.stop();

			/*
			* Calculation End
			 */
			
			Messenger::print("Energy: Correct (test) particle energy is %15.9e kJ/mol\n", interEnergy);
			Messenger::print("Energy: Correct (test) intramolecular energy is %15.9e kJ/mol\n", intraEnergy);
			Messenger::print("Energy: Correct (test) total energy is %15.9e kJ/mol\n", interEnergy + intraEnergy);
			Messenger::print("Energy: Time to do total (test) energy was %s.\n", timer.timeString());
		}
		else
		{
			/*
			* Calculates the total energy of the entire system.
			* 
			* This is a serial routine (subroutines called from within are parallel).
			 */

			Messenger::print("Energy: Calculating total energy for Configuration '%s'...\n", cfg->name());

			// Calculate Grain energy
			Timer interTimer;
			interEnergy = duq.interatomicEnergy(procPool, cfg);
			interTimer.stop();

			// Calculate intramolecular and interGrain correction energy
			Timer intraTimer;
			intraEnergy = duq.intramolecularEnergy(procPool, cfg);
			intraTimer.stop();

			Messenger::print("Energy: Time to do interatomic energy was %s, intramolecular energy was %s.\n", interTimer.timeString(), intraTimer.timeString());
			Messenger::print("Energy: Total Energy (World) is %15.9e kJ/mol (%15.9e kJ/mol interatomic + %15.9e kJ/mol intramolecular)\n", interEnergy + intraEnergy, interEnergy, intraEnergy);

			// Store energies in the Configuration in case somebody else needs them
			GenericListHelper< Array<double> >::realise(cfg->moduleData(), "Inter", uniqueName()).add(interEnergy);
			GenericListHelper< Array<double> >::realise(cfg->moduleData(), "Intra", uniqueName()).add(intraEnergy);
			Array<double>& totalEnergyArray = GenericListHelper< Array<double> >::realise(cfg->moduleData(), "Total", uniqueName());
			totalEnergyArray.add(interEnergy+intraEnergy);

			// Determine stability of energy
			// Check number of points already stored for the Configuration
			double grad = 0.0;
			bool stable = false;
			if (stabilityWindow > totalEnergyArray.nItems()) Messenger::print("Energy: Too few points to assess stability.\n");
			else
			{
				// Work out standard deviation of energy points
				double Sx = 0.0, Sy = 0.0, Sxy = 0.0;
				double xBar = 0.0, yBar = 0.0;
				// -- Calculate mean values
				for (int n=totalEnergyArray.nItems()-stabilityWindow; n<totalEnergyArray.nItems(); ++n)
				{
					xBar += n;
					yBar += totalEnergyArray.value(n);
				}
				xBar /= stabilityWindow;
				yBar /= stabilityWindow;
				// -- Determine Sx, Sy, and Sxy
				for (int n=totalEnergyArray.nItems()-stabilityWindow; n<totalEnergyArray.nItems(); ++n)
				{
					Sx += (n - xBar)*(n - xBar);
					Sy += (totalEnergyArray.value(n) - yBar)*(totalEnergyArray.value(n) - yBar);
					Sxy += (n - xBar) * (totalEnergyArray.value(n) - yBar);
				}
				grad = Sxy / Sx;
				double thresholdValue = fabs(stabilityThreshold*yBar);
				stable = fabs(grad) < thresholdValue;

				// Set variable in Configuration and print output
				GenericListHelper<bool>::realise(cfg->moduleData(), "EnergyStable") = stable;
				Messenger::print("Energy: Gradient of last %i points is %e kJ/mol/step (absolute threshold value is %e, stable = %s).\n", stabilityWindow, grad, thresholdValue, DUQSys::btoa(stable));
			}

			// If writing to a file, append it here
			if (saveData)
			{
				LineParser parser;
				CharString filename("%s-energy.txt", cfg->niceName());

				if (!DUQSys::fileExists(filename))
				{
					parser.openOutput(filename);
					parser.writeLineF("# Energies for Configuration '%s'.\n", cfg->name());
					parser.writeLine("# All values in kJ/mol.\n");
					parser.writeLine("# Iteration   Total         Inter         Intra         Gradient      S?\n");
				}
				else parser.appendOutput(filename);
				parser.writeLineF("  %10i  %12.6e  %12.6e  %12.6e  %12.6e  %i\n", duq.iteration(), interEnergy+intraEnergy, interEnergy, intraEnergy, grad, stable);
				parser.closeFiles();
			}
		}
	}

	return true;
}

// Execute post-processing stage
bool Energy::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
