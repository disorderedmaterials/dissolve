/*
	*** dUQ Methods - Molecular Dynamics
	*** src/modules/md.cpp
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

#include "modules/md.h"
#include "main/duq.h"
#include "classes/grain.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "base/timer.h"


// Static Members
List<Module> MDModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
MDModule::MDModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	options_.add("CapForces", 1.0e7, "Set cap on allowable force (kJ/mol) per atom per axis (or -ve to inhibit)");
	options_.add("CutoffDistance", -1.0, "Interatomic cutoff distance to employ");
	options_.add("DeltaT", 1.0e-4, "Timestep (ps) to use in MD simulation");
	options_.add("EnergyFrequency", 10, "Frequency at which to calculate total system energy (or 0 to inhibit)");
	options_.add("NSteps", 100, "Number of MD steps to perform");
	options_.add("OutputFrequency", 5, "Frequency at which to output step information (or 0 to inhibit)");
	options_.add("RandomVelocities", bool(false), "Whether random velocities should always be assigned before beginning MD simulation");
	options_.add("TrajectoryFrequency", 0, "Write frequency for trajectory file (or 0 to inhibit)");
}

// Destructor
MDModule::~MDModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& MDModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* MDModule::createInstance()
{
	return new MDModule;
}

/*
 * Definition
 */

// Return name of module
const char* MDModule::name()
{
	return "MD";
}

// Return brief description of module
const char* MDModule::brief()
{
	return "Evolve a Configuration using molecular dynamics";
}

// Return instance type for module
Module::InstanceType MDModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool MDModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool MDModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool MDModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* MDModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool MDModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int MDModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int MDModule::nTargetableConfigurations()
{
	return 1;
}


/*
 * Method
 */

// Perform setup tasks for module
bool MDModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool MDModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool MDModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform Molecular Dynamics on a given Configuration.
	 * 
	 * This is a parallel routine, with processes operating in groups.
	 */

	// Get target Configuration
	Configuration* cfg = targetConfigurations_.firstItem();
	GenericList& moduleData = cfg->moduleData();

	// Retrieve control parameters from Configuration
	const double maxForce = GenericListHelper<double>::retrieve(cfg->moduleData(), "CapForces", uniqueName(), options_.valueAsDouble("CapForces")) * 100.0;		// To convert from kJ/mol to 10 J/mol
	double cutoffDistance = GenericListHelper<double>::retrieve(cfg->moduleData(), "CutoffDistance", uniqueName(), options_.valueAsDouble("CutoffDistance"));
	if (cutoffDistance < 0.0) cutoffDistance = duq.pairPotentialRange();
	const double cutoffSq = cutoffDistance * cutoffDistance;
	double deltaT = GenericListHelper<double>::retrieve(cfg->moduleData(), "DeltaT", uniqueName(), options_.valueAsDouble("DeltaT"));
	const int energyFrequency = GenericListHelper<int>::retrieve(cfg->moduleData(), "EnergyFrequency", uniqueName(), options_.valueAsInt("EnergyFrequency"));
	const int nSteps = GenericListHelper<int>::retrieve(cfg->moduleData(), "NSteps", uniqueName(), options_.valueAsInt("NSteps"));
	const int outputFrequency = GenericListHelper<int>::retrieve(cfg->moduleData(), "OutputFrequency", uniqueName(), options_.valueAsInt("OutputFrequency"));
	bool randomVelocities = GenericListHelper<bool>::retrieve(cfg->moduleData(), "RandomVelocities", uniqueName(), options_.valueAsBool("RandomVelocities"));
	const int trajectoryFrequency = GenericListHelper<int>::retrieve(cfg->moduleData(), "TrajectoryFrequency", uniqueName(), options_.valueAsInt("TrajectoryFrequency"));
	bool writeTraj = trajectoryFrequency > 0;
	const double temperature = cfg->temperature();

	// Print argument/parameter summary
	Messenger::print("MD: Cutoff distance is %f\n", cutoffDistance);
	Messenger::print("MD: Number of steps = %i\n", nSteps);
	Messenger::print("MD: Timestep = %10.3e ps\n", deltaT);
	if (writeTraj) Messenger::print("MD: Trajectory file will be appended every %i step(s).\n", trajectoryFrequency);
	else Messenger::print("MD: Trajectory file off.\n");
	if (maxForce > 0) Messenger::print("MD: Forces will be capped to %10.3e kJ/mol per atom per axis.\n", maxForce / 100.0);
	else Messenger::print("MD: Energy will be not be calculated.\n");
	if (energyFrequency > 0) Messenger::print("MD: Energy will be calculated every %i step(s).\n", energyFrequency);
	else Messenger::print("MD: Energy will be not be calculated.\n");
	if (outputFrequency > 0) Messenger::print("MD: Summary will be written every %i step(s).\n", outputFrequency);
	else Messenger::print("MD: Summary will not be written.\n");

	// Create force arrays as simple double arrays (easier to sum with MPI) - others are Vec3<double> arrays
	Array<double> mass(cfg->nAtoms()), fx(cfg->nAtoms()), fy(cfg->nAtoms()), fz(cfg->nAtoms());
	Array< Vec3<double> > a(cfg->nAtoms()), deltaR(cfg->nAtoms());

	// Variables
	int n, nCapped = 0;
	Atom* atoms = cfg->atoms();
	Atom* i;
	double maxDelta, deltaSq, tInstant, ke, tScale, peInter, peIntra;
	double deltaTSq = deltaT*deltaT;

	/*
	 * Calculation Begins
	 */
	
	// Read in or assign random velocities
	// Realise the velocity array from the moduleData
	bool created;
	Array< Vec3<double> >& v = GenericListHelper< Array< Vec3<double> > >::realise(moduleData, "Velocities", uniqueName(), &created);
	if (created)
	{
		randomVelocities = true;
		v.initialise(cfg->nAtoms());
	}
	if (randomVelocities) Messenger::print("MD: Random initial velocities will be assigned.\n");
	else Messenger::print("MD: Existing velocities will be used.\n");

	Vec3<double> vCom;
	double massSum = 0.0;
	for (n=0; n<cfg->nAtoms(); ++n)
	{
		i = &atoms[n];
		if (randomVelocities)
		{
			v[n].x = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
			v[n].y = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
			v[n].z = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
		}

		// Grab atom mass for future use
		mass[n] = PeriodicTable::element(i->element()).isotope(0)->atomicWeight();

		// Calculate total velocity and mass over all atoms
		vCom += v[n] * mass[n];
		massSum += mass[n];
	}

	// Remove any velocity shift
	vCom /= massSum;
	v -= vCom;

	// Calculate instantaneous temperature
        // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
        // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
	const double kb = 0.8314462;
	ke = 0.0;
	for (n=0; n<cfg->nAtoms(); ++n) ke += 0.5 * mass[n] * v[n].dp(v[n]);
	tInstant = ke * 2.0 / (3.0 * cfg->nAtoms() * kb);
	
	// Rescale velocities for desired temperature
	tScale = sqrt(temperature / tInstant);
	for (n=0; n<cfg->nAtoms(); ++n) v[n] *= tScale;

	// Open trajectory file (if requested)
	LineParser trajParser;
	if (writeTraj)
	{
		CharString trajectoryFile = cfg->name();
		trajectoryFile.strcat(".md.xyz");
		if (procPool.isMaster())
		{
			if ((!trajParser.appendOutput(trajectoryFile)) || (!trajParser.isFileGoodForWriting()))
			{
				Messenger::error("Failed to open MD trajectory output file '%s'.\n", trajectoryFile.get());
				procPool.stop();
				return false;
			}
			procPool.proceed();
		}
		else if (!procPool.decision()) return false;
	}

	// Write header
	if (outputFrequency > 0)
	{
		Messenger::print("MD:                                             Energies (kJ/mol)\n");
		Messenger::print("MD:  Step             T(K)         Kinetic      Inter        Intra        Total      deltaT(ps)\n");
	}

	// Start a timer and reset the ProcessPool's time accumulator
	Timer timer;
	timer.start();
	procPool.resetAccumulatedTime();

	// Ready to do MD propagation of system
	for (int step=1; step<=nSteps; ++step)
	{
		// Velocity Verlet first stage (A)
		// A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
		// A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
		// B:  a(t+dt) = F(t+dt)/m
		// B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
		for (n=0; n<cfg->nAtoms(); ++n)
		{
			i = &atoms[n];

			// Propagate positions (by whole step)...
			i->translateCoordinates(v[n]*deltaT + a[n]*0.5*deltaTSq);

			// ...velocities (by half step)...
			v[n] += a[n]*0.5*deltaT;
		}

		// Grain coordinates will have changed...
		cfg->updateAtomsInCells();

		// Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (internal MD units)
		fx = 0.0;
		fy = 0.0;
		fz = 0.0;
		duq.totalForces(procPool, cfg, fx, fy, fz);
		fx *= 100.0;
		fy *= 100.0;
		fz *= 100.0;

		// Cap forces (per atom, per axis)
		if (maxForce > 0)
		{
			for (n=0; n<cfg->nAtoms(); ++n)
			{
				if (fx[n] > maxForce)
				{
					fx[n] = maxForce;
					++nCapped;
				}
				else if (fx[n] < -maxForce)
				{
					fx[n] = -maxForce;
					++nCapped;
				}
				if (fy[n] > maxForce)
				{
					fy[n] = maxForce;
					++nCapped;
				}
				else if (fy[n] < -maxForce)
				{
					fy[n] = -maxForce;
					++nCapped;
				}
				if (fz[n] > maxForce)
				{
					fz[n] = maxForce;
					++nCapped;
				}
				else if (fz[n] < -maxForce)
				{
					fz[n] = -maxForce;
					++nCapped;
				}
			}
		}

		// Velocity Verlet second stage (B) and velocity scaling
		// A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
		// A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
		// B:  a(t+dt) = F(t+dt)/m
		// B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
		ke = 0.0;
		for (n=0; n<cfg->nAtoms(); ++n)
		{
			// Determine new accelerations
			a[n].set(fx[n], fy[n], fz[n]);
			a[n] /= mass[n];

			// ..and finally velocities again (by second half-step)
			v[n] += a[n]*0.5*deltaT;
			
			ke += 0.5 * mass[n] * v[n].dp(v[n]);
		}

		// Rescale velocities for desired temperature
		tInstant = ke * 2.0 / (3.0 * cfg->nAtoms() * kb);
		tScale = sqrt(temperature / tInstant);
		v *= tScale;
		
		// Convert ke from 10J/mol to kJ/mol
		ke *= 0.01;

		// Write step summary?
		if ((step == 1)|| (step%outputFrequency == 0))
		{
			// Include total energy term?
			if ((energyFrequency > 0) && (step%energyFrequency == 0))
			{
				peInter = duq.interatomicEnergy(procPool, cfg);
				peIntra = duq.intramolecularEnergy(procPool, cfg);
				Messenger::print("MD:  %-10i    %10.3e   %10.3e   %10.3e   %10.3e   %10.3e   %10.3e\n", step, tInstant, ke, peInter, peIntra, ke+peIntra+peInter, deltaT);
			}
			else Messenger::print("MD:  %-10i    %10.3e   %10.3e                                          %10.3e\n", step, tInstant, ke, deltaT);
		}

		// Save trajectory frame
		if (writeTraj && (step%trajectoryFrequency == 0))
		{
			if (procPool.isMaster())
			{
				// Write number of atoms
				trajParser.writeLineF("%i\n", cfg->nAtoms());

				// Construct and write header
				CharString header("Step %i of %i, T = %10.3e, ke = %10.3e", step, nSteps, tInstant, ke);
				if ((energyFrequency > 0) && (step%energyFrequency == 0)) header.strcatf(", inter = %10.3e, intra = %10.3e, tot = %10.3e", peInter, peIntra, ke+peInter+peIntra);
				if (!trajParser.writeLineF("%s\n", header.get()))
				{
					procPool.stop();
					return false;
				}

				// Write Atoms
				for (int n=0; n<cfg->nAtoms(); ++n)
				{
					i = cfg->atom(n);
					if (!trajParser.writeLineF("%-3s   %10.3f  %10.3f  %10.3f\n", PeriodicTable::element(i->element()).symbol(), i->r().x, i->r().y, i->r().z))
					{
						procPool.stop();
						return false;
					}
				}

				procPool.proceed();
			}
			else if (!procPool.decision()) return false;
		}
        }
	timer.stop();

	// Close trajectory file
	if (writeTraj && procPool.isMaster()) trajParser.closeFiles();

	if (maxForce > 0) Messenger::print("MD: A total of %i forces were capped over the course of the dynamics (%9.3e per step).\n", nCapped, double(nCapped) / nSteps);
        Messenger::print("MD: %i steps performed (%s work, %s comms)\n", nSteps, timer.totalTimeString(), procPool.accumulatedTimeString());

	// Increment configuration changeCount
	cfg->incrementCoordinateIndex();

	/*
	 * Calculation End
	 */
	
	return true;
}

// Execute post-processing stage
bool MDModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
