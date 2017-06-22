/*
	*** Energy Module
	*** src/modules/energy.cpp
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

#include "modules/energy.h"
#include "main/duq.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Static Members
List<Module> EnergyModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
EnergyModule::EnergyModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Save", bool(true), "Save calculate energy points to the file '<name>.energy.txt'", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("StabilityWindow", 10, "Number of points over which to assess the stability of the energy (per Configuration)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("StabilityThreshold", 0.01, "Threshold value at which energy is deemed stable over the defined windowing period", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Test", bool(false), "Test parallel energy routines against simplified, serial ones", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestAnalytic", bool(false), "Compare parallel energy routines against exact (analytic) energy rather than tabulated values", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestReferenceInter", 0.0, "Reference value for interatomic energy against which to test calculated value", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestReferenceIntra", 0.0, "Reference value for intramolecular energy against which to test calculated value", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestThreshold", 1.0e-1, "Threshold of energy at which test comparison will fail", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
}

// Destructor
EnergyModule::~EnergyModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& EnergyModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* EnergyModule::createInstance()
{
	return new EnergyModule;
}

/*
 * Definition
 */

// Return name of module
const char* EnergyModule::name()
{
	return "Energy";
}

// Return brief description of module
const char* EnergyModule::brief()
{
	return "Calculate the total energy of a Configuration";
}

// Return instance type for module
Module::InstanceType EnergyModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool EnergyModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool EnergyModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool EnergyModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* EnergyModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool EnergyModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int EnergyModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int EnergyModule::nTargetableConfigurations()
{
	return -1;
}

/*
 * Method
 */

// Perform setup tasks for module
bool EnergyModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool EnergyModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool EnergyModule::process(DUQ& duq, ProcessPool& procPool)
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
		const bool saveData = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Save", uniqueName(), options_.valueAsBool("Save"));
		const double stabilityThreshold = GenericListHelper<double>::retrieve(cfg->moduleData(), "StabilityThreshold", uniqueName(), options_.valueAsDouble("StabilityThreshold"));
		const int stabilityWindow = GenericListHelper<int>::retrieve(cfg->moduleData(), "StabilityWindow", uniqueName(), options_.valueAsInt("StabilityWindow"));
		const bool testAnalytic = GenericListHelper<bool>::retrieve(cfg->moduleData(), "TestAnalytic", uniqueName(), options_.valueAsBool("TestAnalytic"));
		const bool testMode = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Test", uniqueName(), options_.valueAsBool("Test"));
		const double testThreshold = GenericListHelper<double>::retrieve(cfg->moduleData(), "TestThreshold", uniqueName(), options_.valueAsDouble("TestThreshold"));
		bool hasReferenceInter;
		const double testReferenceInter = GenericListHelper<double>::retrieve(cfg->moduleData(), "TestReferenceInter", uniqueName(), options_.valueAsDouble("TestReferenceInter"), &hasReferenceInter);
		bool hasReferenceIntra;
		const double testReferenceIntra = GenericListHelper<double>::retrieve(cfg->moduleData(), "TestReferenceIntra", uniqueName(), options_.valueAsDouble("TestReferenceIntra"), &hasReferenceIntra);

		// Calculate the total energy
		if (testMode)
		{
			/*
			* Calculate the total energy of the system using a basic loop on each process, and then compare with production routines.
			 */

			Messenger::print("Energy: Calculating energy for Configuration '%s' in serial test mode...\n", cfg->name());
			if (testAnalytic) Messenger::print("Energy: Exact, analytical potential will be used.\n");

			/*
			* Test Calculation Begins
			 */

			const PotentialMap& potentialMap = duq.potentialMap();
			double correctInterEnergy = 0.0, correctIntraEnergy = 0.0;

			double r, angle;
			Atom* i, *j, *k;
			Vec3<double> vecji, vecjk;
			Molecule* molN, *molM;
			const Box* box = cfg->box();
			double scale;

			Timer testTimer;
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

						if (testAnalytic) correctInterEnergy += potentialMap.analyticEnergy(molN->atom(ii)->globalTypeIndex(), molN->atom(jj)->globalTypeIndex(), box->minimumDistance(molN->atom(ii), molN->atom(jj)));
						else correctInterEnergy += potentialMap.energy(molN->atom(ii)->globalTypeIndex(), molN->atom(jj)->globalTypeIndex(), box->minimumDistance(molN->atom(ii), molN->atom(jj)));
					}
				}

				for (int m=n+1; m<cfg->nMolecules(); ++m)
				{
					molM = cfg->molecule(m);

					// Double loop over atoms
					for (int ii = 0; ii <molN->nAtoms(); ++ii)
					{
						for (int jj = 0; jj <molM->nAtoms(); ++jj)
						{
							if (testAnalytic) correctInterEnergy += potentialMap.analyticEnergy(molN->atom(ii)->globalTypeIndex(), molM->atom(jj)->globalTypeIndex(), box->minimumDistance(molN->atom(ii), molM->atom(jj)));
							else correctInterEnergy += potentialMap.energy(molN->atom(ii)->globalTypeIndex(), molM->atom(jj)->globalTypeIndex(), box->minimumDistance(molN->atom(ii), molM->atom(jj)));
						}
					}
				}

				// Bond energy
				Species* sp = molN->species();
				for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
				{
					// Grab pointers to atoms involved in bond
					i = molN->atom(b->indexI());
					j = molN->atom(b->indexJ());

					r = cfg->box()->minimumDistance(i, j);
					correctIntraEnergy += b->energy(r);
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
					correctIntraEnergy += a->energy(angle);
				}
			}
			testTimer.stop();

			Messenger::print("Energy: Correct interatomic pairpotential energy is %15.9e kJ/mol\n", correctInterEnergy);
			Messenger::print("Energy: Correct intramolecular energy is %15.9e kJ/mol\n", correctIntraEnergy);
			Messenger::print("Energy: Correct total energy is %15.9e kJ/mol\n", correctInterEnergy + correctIntraEnergy);
			Messenger::print("Energy: Time to do total (test) energy was %s.\n", testTimer.totalTimeString());

			/*
			 * Test Calculation End
			 */

			/*
			 * Production Calculation Begins
			 */

			Messenger::print("\nEnergy: Calculating total energy for Configuration '%s'...\n", cfg->name());

			// Calculate interatomic energy
			Timer interTimer;
			double interEnergy = duq.interatomicEnergy(procPool, cfg);
			interTimer.stop();

			// Calculate intramolecular energy
			Timer intraTimer;
			double intraEnergy = duq.intramolecularEnergy(procPool, cfg);
			intraTimer.stop();

			Messenger::print("Energy: Production interatomic pairpotential energy is %15.9e kJ/mol\n", interEnergy);
			Messenger::print("Energy: Production intramolecular energy is %15.9e kJ/mol\n", intraEnergy);
			Messenger::print("Energy: Total production energy is %15.9e kJ/mol\n", interEnergy + intraEnergy);
			Messenger::print("Energy: Time to do interatomic energy was %s.\n", interTimer.totalTimeString());
			Messenger::print("Energy: Time to do intramolecular energy was %s.\n", intraTimer.totalTimeString());

			/*
			 * Production Calculation Ends
			 */

			// Compare production vs reference values
			double delta;
			if (hasReferenceInter)
			{
				Messenger::print("\nEnergy: Reference interatomic energy is %15.9e kJ/mol.\n", testReferenceInter);

				delta = testReferenceInter - correctInterEnergy;
				Messenger::print("Energy: Reference interatomic energy delta with correct value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;

				delta = testReferenceInter - interEnergy;
				Messenger::print("Energy: Reference interatomic energy delta with production value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
			}
			if (hasReferenceIntra)
			{
				Messenger::print("Energy: Reference intramolecular energy is %15.9e kJ/mol.\n", testReferenceIntra);

				delta = testReferenceIntra - correctIntraEnergy;
				Messenger::print("Energy: Reference intramolecular energy delta with correct value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;

				delta = testReferenceIntra - intraEnergy;
				Messenger::print("Energy: Reference intramolecular energy delta with production value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
			}

			// Compare production vs 'correct' values
			double interDelta = correctInterEnergy-interEnergy;
			double intraDelta = correctIntraEnergy-intraEnergy;
			Messenger::print("\nEnergy: Comparing 'correct' with production values...\n");
			Messenger::print("Energy: Interatomic energy delta is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", interDelta, fabs(interDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);
			Messenger::print("Energy: Intramolecular energy delta is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", intraDelta, fabs(intraDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);

			// All OK?
			if (!procPool.allTrue( (fabs(interDelta) < testThreshold) && (fabs(intraDelta) < testThreshold) )) return false;
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
			double interEnergy = duq.interatomicEnergy(procPool, cfg);
			interTimer.stop();

			// Calculate intramolecular and interGrain correction energy
			Timer intraTimer;
			double intraEnergy = duq.intramolecularEnergy(procPool, cfg);
			intraTimer.stop();

			Messenger::print("Energy: Time to do interatomic energy was %s, intramolecular energy was %s.\n", interTimer.totalTimeString(), intraTimer.totalTimeString());
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
				CharString filename("%s.energy.txt", cfg->niceName());

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
bool EnergyModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
