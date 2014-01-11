/*
	*** dUQ Methods - Molecular Dynamics
	*** src/lib/main/methods_md.cpp
	Copyright T. Youngs 2012-2013

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

#include "main/duq.h"
#include "classes/grain.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/forcekernel.h"
#include "base/timer.h"

/*!
 * \brief Perform some MD
 * \details Evolve the current Configuration using a simple molecular dynamics algorithm.
 * 
 * This is a parallel routine, with processes operating in process groups.
 */
CommandReturnValue DUQ::md(Configuration& cfg)
{
	// Get method arguments and parameters from Command
	bool result;
	// -- Number of MD steps to perform
	int nSteps = commandArgumentAsInteger("md", "n", result);
	if (!result) return CommandFail;
	// -- Energy cutoff to employ
	double cutoffSq = commandArgumentAsDouble("md", "cut", result);
	if (!result) return CommandFail;
	if (cutoffSq < 0.0) cutoffSq = pairPotentialRange_;
	// -- Timestep to use
	double deltaT = commandArgumentAsDouble("md", "deltat", result);
	if (!result) return CommandFail;
	// -- Trajectory file
	Dnchar trajFile = commandParameterAsConstChar("md", "trajectory", result);
	if (!result) return CommandFail;
	bool writeTraj = !trajFile.isEmpty();
	// -- Energy calculation at each step
	bool calcEnergy = commandParameterAsInteger("md", "energy", result);
	if (!result) return CommandFail;
	// -- Energy calculation at each step
	int writeFreq = commandParameterAsInteger("md", "freq", result);
	if (!result) return CommandFail;

	// Print summary of parameters
	msg.print("MD: Number of steps = %i\n", nSteps);
	msg.print("MD: Cutoff distance is %f\n", cutoffSq);
	msg.print("MD: Timestep = %10.3e ps\n", deltaT);
	if (writeTraj) msg.print("MD: Trajectory file '%s' will be written.\n", trajFile.get());
	else msg.print("MD: Trajectory file off.\n");
	msg.print("MD: Energy %s be calculated at each step.\n", calcEnergy ? "will" : "will not");
	msg.print("MD: Summary will be written every %i step(s).\n", writeFreq);

	// Create force arrays as simple double arrays (easier to sum with MPI) - others are Vec3<double> arrays
	Array<double> mass(cfg.nAtoms()), fx(cfg.nAtoms()), fy(cfg.nAtoms()), fz(cfg.nAtoms());
	Array< Vec3<double> > a(cfg.nAtoms()), v(cfg.nAtoms()), deltaR(cfg.nAtoms());

	// Variables
	int n, maxDeltaId;
	Atom* atoms = cfg.atoms();
	Atom* i;
	double maxDelta, deltaSq, massSum, tInstant, ke, tScale, pe;
	double deltaTSq = deltaT*deltaT;
	Vec3<double> vCom;
	double maxForce = 100.0;

	/*
	 * Calculation Begins
	 */
	
	// Assign random velocities to start... (grab atomic masses at the same time...)
	msg.print("Assigning random velocities...\n");
	vCom.zero();
	massSum = 0.0;
	for (n=0; n<cfg.nAtoms(); ++n)
	{
		i = cfg.atom(n);
		v[n].x = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
		v[n].y = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
		v[n].z = exp(DUQMath::random()-0.5) / sqrt(TWOPI);
		mass[n] = PeriodicTable::element(i->element()).isotope(0)->atomicWeight();
		vCom += v[n] * mass[n];
		massSum += mass[n];
	}

	// Remove velocity shift
	vCom /= massSum;
	v -= vCom;

	// Calculate instantaneous temperature
        // J = kg m2 s-2  -->   10 J = g Ang2 ps-2
        // If ke is in units of [g mol-1 Angstroms2 ps-2] then must use kb in units of 10 J mol-1 K-1 (= 0.8314462)
	const double kb = 0.8314462;
	ke = 0.0;
	pe = 0.0;
	for (n=0; n<cfg.nAtoms(); ++n) ke += 0.5 * mass[n] * v[n].dp(v[n]);
	tInstant = ke * 2.0 / (3.0 * cfg.nAtoms() * kb);
	
	// Rescale velocities for desired temperature
	tScale = sqrt(temperature_ / tInstant);
	for (n=0; n<cfg.nAtoms(); ++n) v[n] *= tScale;

	// Open trajectory file (if requested)
	LineParser trajParser;
	if (writeTraj)
	{
		if (Comm.master())
		{
			if ((!trajParser.openOutput(trajFile, true)) || (!trajParser.isFileGoodForWriting()))
			{
				msg.error("Failed to open MD trajectory output file '%s'.\n", trajFile.get());
				Comm.decide(false);
				return CommandFail;
			}
			Comm.decide(true);
		}
		else if (!Comm.decision()) return CommandFail;
	}

	// Write header to screen
	if (calcEnergy) msg.print("  Step             T(K)      K.E.(kJ/mol) P.E.(kJ/mol) Etot(kJ/mol)\n");
	else msg.print("  Step             T(K)      K.E.(kj/mol)\n");

	// Start a timer
	Timer timer;

	// Ready to do MD propagation of system
	timer.start();
	Comm.resetAccumulatedTime();
	for (int step=0; step<nSteps; ++step)
	{
// 		 deltaT = 0.001;
// 		 deltaTSq = deltaT*deltaT;
// 		// Velocity Verlet first stage (A) and zero forces
// 		// A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
// 		// A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
// 		// B:  a(t+dt) = F(t+dt)/m
// 		// B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
// 		maxDelta = 0.0;
// 		maxDeltaId = -1;
// 		for (n=0; n<cfg.nAtoms(); ++n)
// 		{
// 			// Calculate position deltas and determine maximum displacement for current timestep
// 			deltaR[n] = v[n]*deltaT + a[n]*0.5*deltaTSq;
// 			deltaSq = deltaR[n].magnitudeSq();
// 			if (deltaSq > maxDelta)
// 			{
// 				maxDelta = deltaSq;
// 				maxDeltaId = n;
// 			}
// 		}
// 		maxDelta = sqrt(maxDelta);
// 		msg.print("Current timestep (%e) will give a maximum displacement of %f Angstroms\n", deltaT, maxDelta);
// 		if (step > 0)
// 		{
// 			do
// 			{
// 				deltaT *= (v[maxDeltaId]*deltaT + a[maxDeltaId]*0.5*deltaTSq).magnitude() > maxDisplacement ? 0.99 : 1.01;
// 				deltaTSq = deltaT*deltaT;
// 			} while (fabs((v[maxDeltaId]*deltaT + a[maxDeltaId]*0.5*deltaTSq).magnitude() - maxDisplacement) > (maxDisplacement*0.05));
// 			// Adjust timestep to give maximum movement and avoid explosion
// 			// dR/dT = v + a*deltaT
// // 			printf("Deriv = %f %f\n", (v[maxDeltaId]+a[maxDeltaId]*deltaT).magnitude(), (v[maxDeltaId]+a[maxDeltaId]*deltaT).magnitude() / (maxDelta - 0.1));
// // 			deltaT = 1.0 / ((v[maxDeltaId]+a[maxDeltaId]*deltaT).magnitude() / 0.1);
// // 			printf("xxx = %f %f\n", 1.0 / deltaT, (maxDelta/maxDisplacement) * (v[maxDeltaId]+a[maxDeltaId]*deltaT).magnitude());
// // // 			deltaT = 0.000564480;
// // 			deltaT = 1.0 / ((maxDelta/maxDisplacement) * (v[maxDeltaId]+a[maxDeltaId]*deltaT).magnitude());
// // 			deltaT = deltaT/2.0;
// 			printf("New DeltaT = %f\n", deltaT);
// 		}

// 		// TEST - Check max displacement with new deltaT
// 		maxDelta = 0.0;
// 		for (n=0; n<cfg.nAtoms(); ++n)
// 		{
// 			// Calculate position deltas and determine maximum displacement for current timestep
// 			deltaR[n] = v[n]*deltaT + a[n]*0.5*deltaTSq;
// 			deltaSq = deltaR[n].magnitudeSq();
// 			if (deltaSq > maxDelta) maxDelta = deltaSq;
// 		}
// 		printf("New max delta = %f\n", sqrt(maxDelta));
		
		for (n=0; n<cfg.nAtoms(); ++n)
		{
			i = cfg.atom(n);

			// Propagate positions (by whole step)...
			i->translateCoordinates(v[n]*deltaT + a[n]*0.5*deltaTSq);

			// ...velocities (by half step)...
			v[n] += a[n]*0.5*deltaT;
			
			// Zero force ready for next calculation
			fx[n] = 0.0;
			fy[n] = 0.0;
			fz[n] = 0.0;
		}

		// Grain coordinates will have changed...
		updateGrains(cfg);

		// Calculate forces - must multiply by 100.0 to convert from kJ/mol to 10J/mol (internal MD units)
		totalForces(cfg, fx, fy, fz, cutoffSq);
		fx *= 100.0;
		fy *= 100.0;
		fz *= 100.0;

// 		// Cap forces
// 		for (n=0; n<cfg.nAtoms(); ++n)
// 		{
// 			// Calculate position deltas and determine maximum displacement for current timestep
// 			if (fx[n] < maxForce) fx[n] = -maxForce;
// 			else if (fx[n] > maxForce) fx[n] = maxForce;
// 			if (fy[n] < maxForce) fy[n] = -maxForce;
// 			else if (fy[n] > maxForce) fy[n] = maxForce;
// 			if (fz[n] < maxForce) fz[n] = -maxForce;
// 			else if (fz[n] > maxForce) fz[n] = maxForce;
// 		}

		// Velocity Verlet second stage (B) and velocity scaling
		// A:  r(t+dt) = r(t) + v(t)*dt + 0.5*a(t)*dt**2
		// A:  v(t+dt/2) = v(t) + 0.5*a(t)*dt
		// B:  a(t+dt) = F(t+dt)/m
		// B:  v(t+dt) = v(t+dt/2) + 0.5*a(t+dt)*dt
		ke = 0.0;
		for (n=0; n<cfg.nAtoms(); ++n)
		{
			// Determine new accelerations
			a[n].set(fx[n], fy[n], fz[n]);
			a[n] /= mass[n];

			// ..and finally velocities again (by second half-step)
			v[n] += a[n]*0.5*deltaT;
			
			ke += 0.5 * mass[n] * v[n].dp(v[n]);
		}

		// Rescale velocities for desired temperature
		tInstant = ke * 2.0 / (3.0 * cfg.nAtoms() * kb);
		tScale = sqrt(temperature_ / tInstant);
		v *= tScale;
		
		// Convert ke from 10J mol-1 to kJ/mol
		ke *= 0.01;

		// Calculate step energy
		if (calcEnergy) pe = intergrainEnergy(cfg) + intramolecularEnergy(cfg);
		
		// Write step summary?
		if (step%writeFreq == 0)
		{
			if (calcEnergy) msg.print("  %-10i    %10.3e   %10.3e   %10.3e   %10.3e\n", step+1, tInstant, ke, pe, ke+pe);
			else msg.print("  %-10i    %10.3e   %10.3e\n", step+1, tInstant, ke);
		}

		// Save trajectory frame
		if (writeTraj)
		{
			if (Comm.master())
			{
				// Write number of atoms
				trajParser.writeLineF("%i\n", cfg.nAtoms());

				// Construct and write header
				Dnchar header(-1, "Step %i of %i, T = %10.3e, ke = %10.3e", step+1, nSteps, tInstant, ke);
				if (calcEnergy) header.strcatf(", pe = %10.3e, tot = %10.3e", pe, ke+pe);
				trajParser.writeLineF("%s\n", header.get());
				// Write Atoms
				for (int n=0; n<cfg.nAtoms(); ++n)
				{
					i = cfg.atom(n);
					trajParser.writeLineF("%-3s   %10.3f  %10.3f  %10.3f\n", PeriodicTable::element(i->element()).symbol(), i->r().x, i->r().y, i->r().z);
				}
			}
			else if (!Comm.decision()) return CommandFail;
		}
        }
        timer.stop();

	// Close trajectory file
	if (writeTraj && Comm.master()) trajParser.closeFiles();

        msg.print("%i molecular dynamics steps performed (%s work, %s comms)\n", nSteps, timer.timeString(), Comm.accumulatedTimeString());

	// Increment configuration changeCount_
	cfg.incrementChangeCount();

	/*
	 * Calculation End
	 */
	
	return CommandSuccess;
}
