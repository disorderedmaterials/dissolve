/*
	*** Energy Module - Method
	*** src/modules/energy/method.cpp
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

#include "modules/energy/energy.h"
#include "main/duq.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Perform set up tasks for module
bool EnergyModule::setUp(ProcessPool& procPool)
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
		const bool saveData = keywords_.asBool("Save");
		const double stabilityThreshold = keywords_.asDouble("StabilityThreshold");
		const int stabilityWindow = keywords_.asInt("StabilityWindow");
		const bool testAnalytic = keywords_.asBool("TestAnalytic");
		const bool testMode = keywords_.asBool("Test");
		const double testThreshold = keywords_.asDouble("TestThreshold");
		bool hasReferenceInter = keywords_.isSet("TestReferenceInter");
		const double testReferenceInter = keywords_.asDouble("TestReferenceInter");
		bool hasReferenceIntra = keywords_.isSet("TestReferenceIntra");
		const double testReferenceIntra = keywords_.asDouble("TestReferenceIntra");

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
			Vec3<double> vecji, vecjk, veckl;
			Molecule* molN, *molM;
			const Box* box = cfg->box();
			double scale;
			const double cutoff = duq.potentialMap().range();

			Timer testTimer;

			// Calculate interatomic energy in a loop over defined Molecules
			for (int n=0; n<cfg->nMolecules(); ++n)
			{
				molN = cfg->molecule(n);

				// Molecule self-energy
				for (int ii = 0; ii<molN->nAtoms()-1; ++ii)
				{
					i = molN->atom(ii);

// 					Messenger::print("Atom %i r = %f %f %f\n", ii, molN->atom(ii)->r().x, molN->atom(ii)->r().y, molN->atom(ii)->r().z);
					for (int jj = ii+1; jj <molN->nAtoms(); ++jj)
					{
						j = molN->atom(jj);

						// Get interatomic distance
						r = box->minimumDistance(i, j);
						if (r > cutoff) continue;

						// Get intramolecular scaling of atom pair
						scale = i->scaling(j);
						if (scale < 1.0e-3) continue;

						if (testAnalytic) correctInterEnergy += potentialMap.analyticEnergy(i, j, r) * scale;
						else correctInterEnergy += potentialMap.energy(i, j, r) * scale;
					}
				}

				// Molecule-molecule energy
				for (int m=n+1; m<cfg->nMolecules(); ++m)
				{
					molM = cfg->molecule(m);

					// Double loop over atoms
					for (int ii = 0; ii <molN->nAtoms(); ++ii)
					{
						i = molN->atom(ii);

						for (int jj = 0; jj <molM->nAtoms(); ++jj)
						{
							j = molM->atom(jj);

							// Get interatomic distance and check cutoff
							r = box->minimumDistance(i, j);
							if (r > cutoff) continue;

							if (testAnalytic) correctInterEnergy += potentialMap.analyticEnergy(i, j, r);
							else correctInterEnergy += potentialMap.energy(i, j, r);
						}
					}
				}
			}

			// Loop over defined Bonds
			DynamicArrayIterator<Bond> bondIterator(cfg->bonds());
			while (Bond* b = bondIterator.iterate())
			{
				r = cfg->box()->minimumDistance(b->i(), b->j());
				correctIntraEnergy += b->energy(r);
			}

			// Loop over defined Angles
			DynamicArrayIterator<Angle> angleIterator(cfg->angles());
			while (Angle* a = angleIterator.iterate())
			{
				// Get vectors 'j-i' and 'j-k'
				vecji = cfg->box()->minimumVector(a->j(), a->i());
				vecjk = cfg->box()->minimumVector(a->j(), a->k());
				
				// Calculate angle
				vecji.normalise();
				vecjk.normalise();
				angle = Box::angleInDegrees(vecji, vecjk);

				// Determine Angle energy
				correctIntraEnergy += a->energy(angle);
			}

			// Loop over defined Torsions
			DynamicArrayIterator<Torsion> torsionIterator(cfg->torsions());
			while (Torsion* t = torsionIterator.iterate())
			{
				// Get vectors 'j-i', 'j-k' and 'k-l'
				vecji = cfg->box()->minimumVector(t->j(), t->i());
				vecjk = cfg->box()->minimumVector(t->j(), t->k());
				veckl = cfg->box()->minimumVector(t->k(), t->l());

				angle = Box::torsionInDegrees(vecji, vecjk, veckl);

				// Determine Torsion energy
				correctIntraEnergy += t->energy(angle);
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
			double bondEnergy, angleEnergy, torsionEnergy;
			double intraEnergy = duq.intramolecularEnergy(procPool, cfg, bondEnergy, angleEnergy, torsionEnergy);
			intraTimer.stop();

			Messenger::print("Energy: Time to do interatomic energy was %s, intramolecular energy was %s.\n", interTimer.totalTimeString(), intraTimer.totalTimeString());
			Messenger::print("Energy: Total Energy (World) is %15.9e kJ/mol (%15.9e kJ/mol interatomic + %15.9e kJ/mol intramolecular).\n", interEnergy + intraEnergy, interEnergy, intraEnergy);
			Messenger::print("Energy: Intramolecular contributions are - bonds = %15.9e kJ/mol, angles = %15.9e kJ/mol, torsions = %15.9e kJ/mol.\n", bondEnergy, angleEnergy, torsionEnergy);

			// Store current energies in the Configuration in case somebody else needs them
			XYData& interData = GenericListHelper<XYData>::realise(cfg->moduleData(), "Inter", uniqueName(), GenericItem::InRestartFileFlag);
			interData.addPoint(duq.iteration(), interEnergy);
			interData.setObjectName(CharString("%s//%s//Inter", cfg->niceName(), uniqueName()));
			XYData& intraData = GenericListHelper<XYData>::realise(cfg->moduleData(), "Intra", uniqueName(), GenericItem::InRestartFileFlag);
			intraData.addPoint(duq.iteration(), intraEnergy);
			intraData.setObjectName(CharString("%s//%s//Intra", cfg->niceName(), uniqueName()));
			XYData& bondData = GenericListHelper<XYData>::realise(cfg->moduleData(), "Bond", uniqueName(), GenericItem::InRestartFileFlag);
			bondData.addPoint(duq.iteration(), bondEnergy);
			bondData.setObjectName(CharString("%s//%s//Bond", cfg->niceName(), uniqueName()));
			XYData& angleData = GenericListHelper<XYData>::realise(cfg->moduleData(), "Angle", uniqueName(), GenericItem::InRestartFileFlag);
			angleData.addPoint(duq.iteration(), angleEnergy);
			angleData.setObjectName(CharString("%s//%s//Angle", cfg->niceName(), uniqueName()));
			XYData& torsionData = GenericListHelper<XYData>::realise(cfg->moduleData(), "Torsion", uniqueName(), GenericItem::InRestartFileFlag);
			torsionData.addPoint(duq.iteration(), torsionEnergy);
			torsionData.setObjectName(CharString("%s//%s//Torsion", cfg->niceName(), uniqueName()));

			// Append to arrays of total energies
			XYData& totalEnergyArray = GenericListHelper<XYData>::realise(cfg->moduleData(), "Total", uniqueName(), GenericItem::InRestartFileFlag);
			totalEnergyArray.addPoint(duq.iteration(), interEnergy+intraEnergy);
			totalEnergyArray.setObjectName(CharString("%s//%s//Total", cfg->niceName(), uniqueName()));

			// Determine stability of energy
			// Check number of points already stored for the Configuration
			double grad = 0.0;
			bool stable = false;
			if (stabilityWindow > totalEnergyArray.nPoints()) Messenger::print("Energy: Too few points to assess stability.\n");
			else
			{
				// Work out standard deviation of energy points
				double Sx = 0.0, Sy = 0.0, Sxy = 0.0;
				double xBar = 0.0, yBar = 0.0;
				// -- Calculate mean values
				for (int n=totalEnergyArray.nPoints()-stabilityWindow; n<totalEnergyArray.nPoints(); ++n)
				{
					xBar += n;
					yBar += totalEnergyArray.y(n);
				}
				xBar /= stabilityWindow;
				yBar /= stabilityWindow;
				// -- Determine Sx, Sy, and Sxy
				for (int n=totalEnergyArray.nPoints()-stabilityWindow; n<totalEnergyArray.nPoints(); ++n)
				{
					Sx += (n - xBar)*(n - xBar);
					Sy += (totalEnergyArray.y(n) - yBar)*(totalEnergyArray.y(n) - yBar);
					Sxy += (n - xBar) * (totalEnergyArray.y(n) - yBar);
				}
				grad = Sxy / Sx;
				double thresholdValue = fabs(stabilityThreshold*yBar);
				stable = fabs(grad) < thresholdValue;

				Messenger::print("Energy: Gradient of last %i points is %e kJ/mol/step (absolute threshold value is %e, stable = %s).\n", stabilityWindow, grad, thresholdValue, DUQSys::btoa(stable));
			}

			// Set variable in Configuration
			GenericListHelper<double>::realise(cfg->moduleData(), "EnergyGradient", "", GenericItem::InRestartFileFlag) = grad;
			GenericListHelper<bool>::realise(cfg->moduleData(), "EnergyStable", "", GenericItem::InRestartFileFlag) = stable;
			GenericListHelper<XYData>::realise(cfg->moduleData(), "EnergyStability", "", GenericItem::InRestartFileFlag).addPoint(duq.iteration(), stable);

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
					parser.writeLine("# Iteration   Total         Inter         Bond          Angle         Torsion       Gradient      S?\n");
				}
				else parser.appendOutput(filename);
				parser.writeLineF("  %10i  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %i\n", duq.iteration(), interEnergy+intraEnergy, interEnergy, bondEnergy, angleEnergy, torsionEnergy, grad, stable);
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
