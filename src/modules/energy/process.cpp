/*
	*** Energy Module - Processing
	*** src/modules/energy/process.cpp
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

#include "modules/energy/energy.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Run set-up stage
bool EnergyModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// For each Configuration target, add a flag to its moduleData (which is *not* stored in the restart file) that we are targeting it
	for (Configuration* cfg : targetConfigurations_) GenericListHelper<bool>::realise(cfg->moduleData(), "_IsEnergyModuleTarget", "", GenericItem::ProtectedFlag) = true;

	return true;
}

// Run main processing
bool EnergyModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Calculate Energy for the target Configuration(s)
	 * 
	 * This is a parallel routine, with processes operating in groups, unless in TEST mode.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0) return Messenger::error("No configuration targets set for module '%s'.\n", uniqueName());

	// Loop over target Configurations
	for (RefListItem<Configuration>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next())
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item();

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

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

		// Print parameter summary
		if (testMode)
		{
			Messenger::print("Energy: All energies will be calculated in serial test mode and compared to production values.\n");
			if (testAnalytic) Messenger::print("Energy: Exact, analytical potential will be used in test.");
			if (hasReferenceInter) Messenger::print("Energy: Reference interatomic energy is %15.9e kJ/mol.\n", testReferenceInter);
			if (hasReferenceIntra) Messenger::print("Energy: Reference intramolecular energy is %15.9e kJ/mol.\n", testReferenceIntra);
		}

		Messenger::print("\n");

		Messenger::print("Calculating total energy for Configuration '%s'...\n", cfg->name());
		// Calculate the total energy
		if (testMode)
		{
			/*
			 * Calculate the total energy of the system using a basic loop on each process, and then compare with production routines.
			 */

			/*
			 * Test Calculation Begins
			 */

			const PotentialMap& potentialMap = dissolve.potentialMap();
			double correctInterEnergy = 0.0, correctIntraEnergy = 0.0;

			double r, angle;
			Atom* i, *j;
			Vec3<double> vecji, vecjk, veckl;
			Molecule* molN, *molM;
			const Box* box = cfg->box();
			double scale;
			const double cutoff = dissolve.potentialMap().range();

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

				// Bond energy
				DynamicArrayConstIterator<SpeciesBond> bondIterator(molN->species()->constBonds());
				while (const SpeciesBond* b = bondIterator.iterate())
				{
					r = cfg->box()->minimumDistance(molN->atom(b->indexI()), molN->atom(b->indexJ()));
					correctIntraEnergy += b->energy(r);
				}

				// Angle energy
				DynamicArrayConstIterator<SpeciesAngle> angleIterator(molN->species()->constAngles());
				while (const SpeciesAngle* a = angleIterator.iterate())
				{
					// Get vectors 'j-i' and 'j-k'
					vecji = cfg->box()->minimumVector(molN->atom(a->indexJ()), molN->atom(a->indexI()));
					vecjk = cfg->box()->minimumVector(molN->atom(a->indexJ()), molN->atom(a->indexK()));
					
					// Calculate angle
					vecji.normalise();
					vecjk.normalise();
					angle = Box::angleInDegrees(vecji, vecjk);

					// Determine Angle energy
					correctIntraEnergy += a->energy(angle);
				}

				// Torsion energy
				DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(molN->species()->constTorsions());
				while (const SpeciesTorsion* t = torsionIterator.iterate())
				{
					// Get vectors 'j-i', 'j-k' and 'k-l'
					vecji = cfg->box()->minimumVector(molN->atom(t->indexJ()), molN->atom(t->indexI()));
					vecjk = cfg->box()->minimumVector(molN->atom(t->indexJ()), molN->atom(t->indexK()));
					veckl = cfg->box()->minimumVector(molN->atom(t->indexK()), molN->atom(t->indexL()));

					angle = Box::torsionInDegrees(vecji, vecjk, veckl);

					// Determine Torsion energy
					correctIntraEnergy += t->energy(angle);
				}
			}
			testTimer.stop();

			Messenger::print("Correct interatomic pairpotential energy is %15.9e kJ/mol\n", correctInterEnergy);
			Messenger::print("Correct intramolecular energy is %15.9e kJ/mol\n", correctIntraEnergy);
			Messenger::print("Correct total energy is %15.9e kJ/mol\n", correctInterEnergy + correctIntraEnergy);
			Messenger::print("Time to do total (test) energy was %s.\n", testTimer.totalTimeString());

			/*
			 * Test Calculation End
			 */

			/*
			 * Production Calculation Begins
			 */

			// Calculate interatomic energy
			Timer interTimer;
			double interEnergy = interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
			interTimer.stop();

			// Calculate intramolecular energy
			Timer intraTimer;
			double intraEnergy = intraMolecularEnergy(procPool, cfg, dissolve.potentialMap());
			intraTimer.stop();

			Messenger::print("Production interatomic pairpotential energy is %15.9e kJ/mol\n", interEnergy);
			Messenger::print("Production intramolecular energy is %15.9e kJ/mol\n", intraEnergy);
			Messenger::print("Total production energy is %15.9e kJ/mol\n", interEnergy + intraEnergy);
			Messenger::print("Time to do interatomic energy was %s.\n", interTimer.totalTimeString());
			Messenger::print("Time to do intramolecular energy was %s.\n", intraTimer.totalTimeString());

			/*
			 * Production Calculation Ends
			 */

			// Compare production vs reference values
			double delta;
			if (hasReferenceInter)
			{
				delta = testReferenceInter - correctInterEnergy;
				Messenger::print("Reference interatomic energy delta with correct value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;

				delta = testReferenceInter - interEnergy;
				Messenger::print("Reference interatomic energy delta with production value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
			}
			if (hasReferenceIntra)
			{
				delta = testReferenceIntra - correctIntraEnergy;
				Messenger::print("Reference intramolecular energy delta with correct value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;

				delta = testReferenceIntra - intraEnergy;
				Messenger::print("Reference intramolecular energy delta with production value is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
				if (!procPool.allTrue(fabs(delta) < testThreshold)) return false;
			}

			// Compare production vs 'correct' values
			double interDelta = correctInterEnergy-interEnergy;
			double intraDelta = correctIntraEnergy-intraEnergy;
			Messenger::print("Comparing 'correct' with production values...\n");
			Messenger::print("Interatomic energy delta is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", interDelta, fabs(interDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);
			Messenger::print("Intramolecular energy delta is %15.9e kJ/mol and is %s (threshold is %10.3e kJ/mol)\n", intraDelta, fabs(intraDelta) < testThreshold ? "OK" : "NOT OK", testThreshold);

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

			procPool.resetAccumulatedTime();

			// Calculate intermolecular energy
			Timer interTimer;
			double interEnergy = interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
			interTimer.stop();

			// Calculate intramolecular and intermolecular correction energy
			Timer intraTimer;
			double bondEnergy, angleEnergy, torsionEnergy;
			double intraEnergy = intraMolecularEnergy(procPool, cfg, dissolve.potentialMap(), bondEnergy, angleEnergy, torsionEnergy);
			intraTimer.stop();

			Messenger::print("Time to do interatomic energy was %s, intramolecular energy was %s (%s comms).\n", interTimer.totalTimeString(), intraTimer.totalTimeString(), procPool.accumulatedTimeString());
			Messenger::print("Total Energy (World) is %15.9e kJ/mol (%15.9e kJ/mol interatomic + %15.9e kJ/mol intramolecular).\n", interEnergy + intraEnergy, interEnergy, intraEnergy);
			Messenger::print("Intramolecular contributions are - bonds = %15.9e kJ/mol, angles = %15.9e kJ/mol, torsions = %15.9e kJ/mol.\n", bondEnergy, angleEnergy, torsionEnergy);

			// Store current energies in the Configuration in case somebody else needs them
			Data1D& interData = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Inter", uniqueName(), GenericItem::InRestartFileFlag);
			interData.addPoint(dissolve.iteration(), interEnergy);
			interData.setObjectTag(CharString("%s//%s//Inter", cfg->niceName(), uniqueName()));
			Data1D& intraData = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Intra", uniqueName(), GenericItem::InRestartFileFlag);
			intraData.addPoint(dissolve.iteration(), intraEnergy);
			intraData.setObjectTag(CharString("%s//%s//Intra", cfg->niceName(), uniqueName()));
			Data1D& bondData = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Bond", uniqueName(), GenericItem::InRestartFileFlag);
			bondData.addPoint(dissolve.iteration(), bondEnergy);
			bondData.setObjectTag(CharString("%s//%s//Bond", cfg->niceName(), uniqueName()));
			Data1D& angleData = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Angle", uniqueName(), GenericItem::InRestartFileFlag);
			angleData.addPoint(dissolve.iteration(), angleEnergy);
			angleData.setObjectTag(CharString("%s//%s//Angle", cfg->niceName(), uniqueName()));
			Data1D& torsionData = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Torsion", uniqueName(), GenericItem::InRestartFileFlag);
			torsionData.addPoint(dissolve.iteration(), torsionEnergy);
			torsionData.setObjectTag(CharString("%s//%s//Torsion", cfg->niceName(), uniqueName()));

			// Append to arrays of total energies
			Data1D& totalEnergyArray = GenericListHelper<Data1D>::realise(cfg->moduleData(), "Total", uniqueName(), GenericItem::InRestartFileFlag);
			totalEnergyArray.addPoint(dissolve.iteration(), interEnergy+intraEnergy);
			totalEnergyArray.setObjectTag(CharString("%s//%s//Total", cfg->niceName(), uniqueName()));

			// Determine stability of energy
			// Check number of points already stored for the Configuration
			double grad = 0.0;
			bool stable = false;
			if (stabilityWindow > totalEnergyArray.nValues()) Messenger::print("Too few points to assess stability.\n");
			else
			{
				double yMean;
				grad = Regression::linear(totalEnergyArray, stabilityWindow, yMean);
				double thresholdValue = fabs(stabilityThreshold*yMean);
				stable = fabs(grad) < thresholdValue;

				Messenger::print("Gradient of last %i points is %e kJ/mol/step (absolute threshold value is %e, stable = %s).\n", stabilityWindow, grad, thresholdValue, DissolveSys::btoa(stable));
			}

			// Set variable in Configuration
			GenericListHelper<double>::realise(cfg->moduleData(), "EnergyGradient", "", GenericItem::InRestartFileFlag) = grad;
			GenericListHelper<bool>::realise(cfg->moduleData(), "EnergyStable", "", GenericItem::InRestartFileFlag) = stable;
			GenericListHelper<Data1D>::realise(cfg->moduleData(), "EnergyStability", "", GenericItem::InRestartFileFlag).addPoint(dissolve.iteration(), stable);

			// If writing to a file, append it here
			if (saveData)
			{
				LineParser parser;
				CharString filename("%s.energy.txt", cfg->niceName());

				if (!DissolveSys::fileExists(filename))
				{
					parser.openOutput(filename);
					parser.writeLineF("# Energies for Configuration '%s'.\n", cfg->name());
					parser.writeLine("# All values in kJ/mol.\n");
					parser.writeLine("# Iteration   Total         Inter         Bond          Angle         Torsion       Gradient      S?\n");
				}
				else parser.appendOutput(filename);
				parser.writeLineF("  %10i  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %i\n", dissolve.iteration(), interEnergy+intraEnergy, interEnergy, bondEnergy, angleEnergy, torsionEnergy, grad, stable);
				parser.closeFiles();
			}
		}

		Messenger::print("\n");
	}

	return true;
}

