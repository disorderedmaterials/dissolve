/*
	*** Forces Module
	*** src/modules/forces.cpp
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

#include "modules/forces.h"
#include "modules/import.h"
#include "main/duq.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Static Members
List<Module> ForcesModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
ForcesModule::ForcesModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Save", bool(false), "Save forces for the Configuration to the file '<name>.forces.txt'", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Test", bool(false), "Test parallel energy routines against simplified, serial ones", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestAnalytic", bool(false), "Compare parallel energy routines against exact (analytic) energy rather than tabulated values", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestInter", bool(true), "Include interatomic forces in test", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestIntra", bool(true), "Include intramolecular forces in test", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestThreshold", 1.0e-2, "Threshold of energy at which test comparison will fail", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
}

// Destructor
ForcesModule::~ForcesModule()
{
}

/*
 * Instances
 */

// Return list of all created instances of this Module
List<Module>& ForcesModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* ForcesModule::createInstance()
{
	return new ForcesModule;
}

/*
 * Definition
 */

// Return name of module
const char* ForcesModule::name()
{
	return "Forces";
}

// Return brief description of module
const char* ForcesModule::brief()
{
	return "Calculate the total atomic forces within a Configuration";
}

// Return instance type for module
Module::InstanceType ForcesModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool ForcesModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool ForcesModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool ForcesModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for
const char* ForcesModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool ForcesModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ForcesModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	if (DUQSys::sameString(parser.argc(0), "TestReference"))
	{
		Messenger::print("Reading test reference forces.\n");

		// Realise some arrays to store the forces in
		Array<double>& fx = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFX", uniqueName(), GenericItem::NoOutputFlag);
		Array<double>& fy = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFY", uniqueName(), GenericItem::NoOutputFlag);
		Array<double>& fz = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFZ", uniqueName(), GenericItem::NoOutputFlag);

		// Second argument is the format, third (if present) is the target file
		if (parser.hasArg(2))
		{
			LineParser fileParser(&duq->worldPool());
			if (!fileParser.openInput(parser.argc(2))) return 0;

			return ImportModule::readForces(parser.argc(1), fileParser, fx, fy, fz);
		}
		else return ImportModule::readForces(parser.argc(1), parser, fx, fy, fz);
	}

	return -1;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int ForcesModule::nTargetableConfigurations()
{
	return -1;
}

/*
 * Method
 */

// Perform setup tasks for module
bool ForcesModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool ForcesModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool ForcesModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate Forces for the target Configuration(s)
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
		const bool testMode = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Test", uniqueName(), options_.valueAsBool("Test"));
		const bool testAnalytic = GenericListHelper<bool>::retrieve(cfg->moduleData(), "TestAnalytic", uniqueName(), options_.valueAsBool("TestAnalytic"));
		const bool testInter = GenericListHelper<bool>::retrieve(cfg->moduleData(), "TestInter", uniqueName(), options_.valueAsBool("TestInter"));
		const bool testIntra = GenericListHelper<bool>::retrieve(cfg->moduleData(), "TestIntra", uniqueName(), options_.valueAsBool("TestIntra"));
		const double testThreshold = GenericListHelper<double>::retrieve(cfg->moduleData(), "TestThreshold", uniqueName(), options_.valueAsDouble("TestThreshold"));

		// Calculate the total forces
		if (testMode)
		{
			/*
			 * Calculate the total forces in the system using basic loops.
			 * 
			 * This is a serial routine, with all processes independently calculating their own value.
			 */

			Messenger::print("Forces: Calculating forces for Configuration '%s' in serial test mode...\n", cfg->name());
			if (testAnalytic) Messenger::print("Forces: Exact (analytic) forces will be calculated.\n");
			Messenger::print("Forces:: Test threshold for failure is %f%%.\n", testThreshold);

			/*
			 * Calculation Begins
			 */

			const PotentialMap& potentialMap = duq.potentialMap();
			double cutoffSq = potentialMap.range()*potentialMap.range();

			double angle, magjisq, magji, magjk, dp, force, r;
			Atom* i, *j, *k;
			Vec3<double> vecji, vecjk, forcei, forcek;
			Molecule* molN, *molM;
			const Box* box = cfg->box();
			double scale;

			// Allocate the force arrays
			Array<double> interFx(cfg->nAtoms()), interFy(cfg->nAtoms()), interFz(cfg->nAtoms());
			Array<double> intraFx(cfg->nAtoms()), intraFy(cfg->nAtoms()), intraFz(cfg->nAtoms());
			Array<double> referenceFx(cfg->nAtoms()), referenceFy(cfg->nAtoms()), referenceFz(cfg->nAtoms());
			Array<double> checkInterFx(cfg->nAtoms()), checkInterFy(cfg->nAtoms()), checkInterFz(cfg->nAtoms());
			Array<double> checkIntraFx(cfg->nAtoms()), checkIntraFy(cfg->nAtoms()), checkIntraFz(cfg->nAtoms());
			interFx = 0.0;
			interFy = 0.0;
			interFz = 0.0;
			intraFx = 0.0;
			intraFy = 0.0;
			intraFz = 0.0;
			referenceFx = 0.0;
			referenceFy = 0.0;
			referenceFz = 0.0;
			checkInterFx = 0.0;
			checkInterFy = 0.0;
			checkInterFz = 0.0;
			checkIntraFx = 0.0;
			checkIntraFy = 0.0;
			checkIntraFz = 0.0;

			// Calculate interatomic and intramlecular energy in a loop over defined Molecules
			Timer timer;
			for (int n=0; n<cfg->nMolecules(); ++n)
			{
				molN = cfg->molecule(n);

				// Intramolecular forces (excluding bound terms) in molecule N
				if (testInter) for (int ii = 0; ii <molN->nAtoms()-1; ++ii)
				{
					i = molN->atom(ii);

					for (int jj = ii+1; jj <molN->nAtoms(); ++jj)
					{
						// Get intramolecular scaling of atom pair
						scale = molN->species()->scaling(ii, jj);
						if (scale < 1.0e-3) continue;

						j = molN->atom(jj);

						// Determine final forces
						vecji = box->minimumVector(i, j);
						magjisq = vecji.magnitudeSq();
						if (magjisq > cutoffSq) continue;
						r = DUQMath::squareRoot(magjisq);
						vecji /= r;

						if (testAnalytic) vecji *= potentialMap.analyticForce(molN->atom(ii), molN->atom(jj), r);
						else vecji *= potentialMap.force(molN->atom(ii), molN->atom(jj), r);
						interFx[i->index()] += vecji.x;
						interFy[i->index()] += vecji.y;
						interFz[i->index()] += vecji.z;
						interFx[j->index()] -= vecji.x;
						interFy[j->index()] -= vecji.y;
						interFz[j->index()] -= vecji.z;
					}
				}

				// Forces between molecule N and molecule M
				if (testInter) for (int m=n+1; m<cfg->nMolecules(); ++m)
				{
					molM = cfg->molecule(m);

					// Double loop over atoms
					for (int ii=0; ii<molN->nAtoms(); ++ii)
					{
						i = molN->atom(ii);

						for (int jj=0; jj<molM->nAtoms(); ++jj)
						{
							j = molM->atom(jj);

							// Determine final forces
							vecji = box->minimumVector(i, j);
							magjisq = vecji.magnitudeSq();
							if (magjisq > cutoffSq) continue;
							r = DUQMath::squareRoot(magjisq);
							vecji /= r;

							if (testAnalytic) vecji *= potentialMap.analyticForce(i, j, r);
							else vecji *= potentialMap.force(i, j, r);
							
// 							printf("%i  %i  %f  %15.9e %15.9e\n", i->index()+1, j->index()+1, sqrt(magji), potentialMap.force(i->globalTypeIndex(), j->globalTypeIndex(), magji), potentialMap.analyticForce(i->globalTypeIndex(), j->globalTypeIndex(), magji));
							interFx[i->index()] += vecji.x;
							interFy[i->index()] += vecji.y;
							interFz[i->index()] += vecji.z;
							interFx[j->index()] -= vecji.x;
							interFy[j->index()] -= vecji.y;
							interFz[j->index()] -= vecji.z;
						}
					}
				}

				// Bond forces
				Species* sp = molN->species();
				if (testIntra) for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
				{
					// Grab pointers to atoms involved in bond
					i = molN->atom(b->indexI());
					j = molN->atom(b->indexJ());

					// Determine final forces
					vecji = box->minimumVector(i, j);
					r = vecji.magAndNormalise();
					vecji *= b->force(r);
					intraFx[i->index()] -= vecji.x;
					intraFy[i->index()] -= vecji.y;
					intraFz[i->index()] -= vecji.z;
					intraFx[j->index()] += vecji.x;
					intraFy[j->index()] += vecji.y;
					intraFz[j->index()] += vecji.z;
				}

				// Angle forces
				if (testIntra) for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
				{
					// Grab pointers to atoms involved in angle
					i = molN->atom(a->indexI());
					j = molN->atom(a->indexJ());
					k = molN->atom(a->indexK());

					// Get vectors 'j-i' and 'j-k'
					vecji = box->minimumVector(j, i);
					vecjk = box->minimumVector(j, k);
					
					// Calculate angle
					magji = vecji.magAndNormalise();
					magjk = vecjk.magAndNormalise();
					angle = Box::angle(vecji, vecjk, dp);

					// Determine Angle force vectors for atoms
					force = a->force(angle);
					forcei = vecjk - vecji * dp;
					forcei *= force / magji;
					forcek = vecji - vecjk * dp;
					forcek *= force / magjk;
					
					// Store forces
					intraFx[i->index()] += forcei.x;
					intraFy[i->index()] += forcei.y;
					intraFz[i->index()] += forcei.z;
					intraFx[j->index()] -= forcei.x + forcek.x;
					intraFy[j->index()] -= forcei.y + forcek.y;
					intraFz[j->index()] -= forcei.z + forcek.z;
					intraFx[k->index()] += forcek.x;
					intraFy[k->index()] += forcek.y;
					intraFz[k->index()] += forcek.z;
				}
			}
			timer.stop();

			// Convert forces to 10J/mol
			interFx *= 100.0;
			interFy *= 100.0;
			interFz *= 100.0;
			intraFx *= 100.0;
			intraFy *= 100.0;
			intraFz *= 100.0;

			Messenger::print("Forces: Time to do total (test) forces was %s.\n", timer.totalTimeString());

			/*
			 * Test Calculation End
			 */

			/*
			 * Production Calculation Begins
			 */

			Messenger::print("Forces: Calculating total forces for Configuration '%s'...\n", cfg->name());

			// Calculate interatomic forces
			Timer interTimer;
			interTimer.start();
			if (testInter) duq.interatomicForces(procPool, cfg, checkInterFx, checkInterFy, checkInterFz);
			interTimer.stop();
			Messenger::printVerbose("Forces: Time to do interatomic forces was %s.\n", interTimer.totalTimeString());
			
			// Calculate intramolecular forces
			Timer intraTimer;
			intraTimer.start();
			if (testIntra) duq.intramolecularForces(procPool, cfg, checkIntraFx, checkIntraFy, checkIntraFz);
			intraTimer.stop();

			// Convert forces to 10J/mol
			checkInterFx *= 100.0;
			checkInterFy *= 100.0;
			checkInterFz *= 100.0;
			checkIntraFx *= 100.0;
			checkIntraFy *= 100.0;
			checkIntraFz *= 100.0;

			Messenger::print("Forces: Time to do interatomic forces was %s, intramolecular forces was %s.\n", interTimer.totalTimeString(), intraTimer.totalTimeString());

			/*
			 * Production Calculation Ends
			 */

			// Test 'correct' forces against production forces
			int nFailed1 = 0;
			bool failed;
			Vec3<double> interRatio, intraRatio;
			double sumError;

			Messenger::print("Forces: Testing calculated 'correct' forces against calculated production forces - atoms with erroneous forces will be output...\n");
			sumError = 0.0;
			for (int n=0; n<cfg->nAtoms(); ++n)
			{
				interRatio.set(interFx[n] - checkInterFx[n], interFy[n] - checkInterFy[n], interFz[n] - checkInterFz[n]);
				intraRatio.set(intraFx[n] - checkIntraFx[n], intraFy[n] - checkIntraFy[n], intraFz[n] - checkIntraFz[n]);
				if (fabs(interFx[n]) > 1.0e-6) interRatio.x *= 100.0 / interFx[n];
				if (fabs(interFy[n]) > 1.0e-6) interRatio.y *= 100.0 / interFy[n];
				if (fabs(interFz[n]) > 1.0e-6) interRatio.z *= 100.0 / interFz[n];
				if (fabs(intraFx[n]) > 1.0e-6) intraRatio.x *= 100.0 / intraFx[n];
				if (fabs(intraFy[n]) > 1.0e-6) intraRatio.y *= 100.0 / intraFy[n];
				if (fabs(intraFz[n]) > 1.0e-6) intraRatio.z *= 100.0 / intraFz[n];

				if (fabs(intraRatio.x) > testThreshold) failed = true;
				else if (fabs(intraRatio.y) > testThreshold) failed = true;
				else if (fabs(intraRatio.z) > testThreshold) failed = true;
				else if (fabs(interRatio.x) > testThreshold) failed = true;
				else if (fabs(interRatio.y) > testThreshold) failed = true;
				else if (fabs(interRatio.z) > testThreshold) failed = true;
				else failed = false;

				// Sum average errors
				sumError += intraRatio.x + intraRatio.y + intraRatio.z + interRatio.x + interRatio.y + interRatio.z;

				if (failed)
				{
					Messenger::print("Forces: Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e (%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol (inter)\n", n+1, interFx[n] - checkInterFx[n], interRatio.x, interFy[n] - checkInterFy[n], interRatio.y, interFz[n] - checkInterFz[n], interRatio.z);
					Messenger::print("                                     %15.8e (%5.2f%%) %15.8e (%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol (intra)\n", n+1, intraFx[n] - checkIntraFx[n], intraRatio.x, intraFy[n] - checkIntraFy[n], intraRatio.y, intraFz[n] - checkIntraFz[n], intraRatio.z);
					++nFailed1;
				}
			}
			Messenger::print("Forces: Number of atoms with failed force components = %i = %s\n", nFailed1, nFailed1 == 0 ? "OK" : "NOT OK");
			Messenger::print("Forces: Average error in force components was %f%%.\n", sumError / (cfg->nAtoms()*6));

			// Test reference forces against production (if reference forces present)
			int nFailed2 = 0, nFailed3 = 0;
			Vec3<double> totalRatio;
			sumError = 0.0;
			if (cfg->moduleData().contains("ReferenceFX", uniqueName()) && cfg->moduleData().contains("ReferenceFY", uniqueName()) && cfg->moduleData().contains("ReferenceFZ", uniqueName()))
			{
				// Grab reference force arrays and check sizes
				Array<double>& referenceFx = GenericListHelper< Array<double> >::retrieve(cfg->moduleData(), "ReferenceFX", uniqueName());
				if (referenceFx.nItems() != cfg->nAtoms())
				{
					Messenger::error("Number of force components in ReferenceFX is %i, but the Configuration '%s' contains %i atoms.\n", referenceFx.nItems(), cfg->name(), cfg->nAtoms());
					return false;
				}
				Array<double>& referenceFy = GenericListHelper< Array<double> >::retrieve(cfg->moduleData(), "ReferenceFY", uniqueName());
				if (referenceFy.nItems() != cfg->nAtoms())
				{
					Messenger::error("Number of force components in ReferenceFY is %i, but the Configuration '%s' contains %i atoms.\n", referenceFy.nItems(), cfg->name(), cfg->nAtoms());
					return false;
				}
				Array<double>& referenceFz = GenericListHelper< Array<double> >::retrieve(cfg->moduleData(), "ReferenceFZ", uniqueName());
				if (referenceFz.nItems() != cfg->nAtoms())
				{
					Messenger::error("Number of force components in ReferenceFZ is %i, but the Configuration '%s' contains %i atoms.\n", referenceFz.nItems(), cfg->name(), cfg->nAtoms());
					return false;
				}

				Messenger::print("\nForces: Testing reference forces against calculated 'correct' forces - atoms with erroneous forces will be output...\n");
				sumError = 0.0;
				for (int n=0; n<cfg->nAtoms(); ++n)
				{
					totalRatio.x = referenceFx[n] - (interFx[n] + intraFx[n]);
					totalRatio.y = referenceFy[n] - (interFy[n] + intraFy[n]);
					totalRatio.z = referenceFz[n] - (interFz[n] + intraFz[n]);
					if (fabs(interFx[n]+intraFx[n]) > 1.0e-6) totalRatio.x *= 100.0 / (interFx[n]+intraFx[n]);
					if (fabs(interFy[n]+intraFy[n]) > 1.0e-6) totalRatio.y *= 100.0 / (interFy[n]+intraFy[n]);
					if (fabs(interFz[n]+intraFz[n]) > 1.0e-6) totalRatio.z *= 100.0 / (interFz[n]+intraFz[n]);

					if (fabs(totalRatio.x) > testThreshold) failed = true;
					else if (fabs(totalRatio.y) > testThreshold) failed = true;
					else if (fabs(totalRatio.z) > testThreshold) failed = true;
					else failed = false;

					// Sum average errors
					sumError += totalRatio.x + totalRatio.y + totalRatio.z;

					if (failed)
					{
						Messenger::print("Forces: Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e (%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol\n", n+1, referenceFx[n] - (interFx[n] + intraFx[n]), totalRatio.x, referenceFy[n] - (interFy[n] + intraFy[n]), totalRatio.y, referenceFz[n] - (interFz[n] + intraFz[n]), totalRatio.z);
						++nFailed2;
					}
				}
				Messenger::print("Forces: Number of atoms with failed force components = %i = %s\n", nFailed2, nFailed2 == 0 ? "OK" : "NOT OK");
				Messenger::print("Forces: Average error in force components was %f%%.\n", sumError / (cfg->nAtoms()*3));

				Messenger::print("\nForces: Testing reference forces against calculated production forces - atoms with erroneous forces will be output...\n");

				sumError = 0.0;
				for (int n=0; n<cfg->nAtoms(); ++n)
				{
					totalRatio.x = referenceFx[n] - (checkInterFx[n] + checkIntraFx[n]);
					totalRatio.y = referenceFy[n] - (checkInterFy[n] + checkIntraFy[n]);
					totalRatio.z = referenceFz[n] - (checkInterFz[n] + checkIntraFz[n]);
					if (fabs(checkInterFx[n]+checkIntraFx[n]) > 1.0e-6) totalRatio.x *= 100.0 / (checkInterFx[n]+checkIntraFx[n]);
					if (fabs(checkInterFy[n]+checkIntraFy[n]) > 1.0e-6) totalRatio.y *= 100.0 / (checkInterFy[n]+checkIntraFy[n]);
					if (fabs(checkInterFz[n]+checkIntraFz[n]) > 1.0e-6) totalRatio.z *= 100.0 / (checkInterFz[n]+checkIntraFz[n]);

					if (fabs(totalRatio.x) > testThreshold) failed = true;
					else if (fabs(totalRatio.y) > testThreshold) failed = true;
					else if (fabs(totalRatio.z) > testThreshold) failed = true;
					else failed = false;

					// Sum average errors
					sumError += totalRatio.x + totalRatio.y + totalRatio.z;

					if (failed)
					{
						Messenger::print("Forces: Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e (%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol\n", n+1, referenceFx[n] - (checkInterFx[n] + checkIntraFx[n]), totalRatio.x, referenceFy[n] - (checkInterFy[n] + checkIntraFy[n]), totalRatio.y, referenceFz[n] - (checkInterFz[n] + checkIntraFz[n]), totalRatio.z);
						++nFailed3;
					}
				}
				Messenger::print("Forces: Number of atoms with failed force components = %i = %s\n", nFailed3, nFailed3 == 0 ? "OK" : "NOT OK");
				Messenger::print("Forces: Average error in force components was %f%%.\n", sumError / (cfg->nAtoms()*6));
			}

			if (!procPool.allTrue((nFailed1 + nFailed2 + nFailed3) == 0)) return false;
		}
		else
		{
			/*
			 * Calculates the total forces in the system.
			 * 
			 * This is a serial routine (subroutines called from within are parallel).
			 */

			Messenger::print("Forces: Calculating total forces for Configuration '%s'...\n", cfg->name());

			// Realise the force arrays
			Array<double>& fx = GenericListHelper< Array<double> >::realise(cfg->moduleData(), "FX", uniqueName(), GenericItem::NoOutputFlag);
			Array<double>& fy = GenericListHelper< Array<double> >::realise(cfg->moduleData(), "FY", uniqueName(), GenericItem::NoOutputFlag);
			Array<double>& fz = GenericListHelper< Array<double> >::realise(cfg->moduleData(), "FZ", uniqueName(), GenericItem::NoOutputFlag);
			fx.initialise(cfg->nAtoms());
			fy.initialise(cfg->nAtoms());
			fz.initialise(cfg->nAtoms());

			// Calculate interatomic forces
			Timer interTimer;
			interTimer.start();
			duq.interatomicForces(procPool, cfg, fx, fy, fz);
			interTimer.stop();
			Messenger::printVerbose("Forces: Time to do interatomic forces was %s.\n", interTimer.totalTimeString());
			
			// Calculate intramolecular forces
			Timer intraTimer;
			intraTimer.start();
			duq.intramolecularForces(procPool, cfg, fx, fy, fz);
			intraTimer.stop();

			Messenger::print("Forces: Time to do interatomic forces was %s, intramolecular forces was %s.\n", interTimer.totalTimeString(), intraTimer.totalTimeString());

			// If writing to a file, append it here
			if (saveData)
			{
				LineParser parser;
				CharString filename("%s.forces.txt", cfg->niceName());

				parser.openOutput(filename);
				parser.writeLineF("# Forces for Configuration '%s'.\n", cfg->name());
				parser.writeLine("# Force units are 10J/mol.\n");
				parser.writeLine("# Atom        FX            FY            FZ\n");
				for (int n=0; n<cfg->nAtoms(); ++n) parser.writeLineF("  %10i  %12.6e  %12.6e  %12.6e  %12.6e  %i\n", n+1, fx[n], fy[n], fz[n]);
				parser.closeFiles();
			}
		}
	}

	return true;
}

// Execute post-processing stage
bool ForcesModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
