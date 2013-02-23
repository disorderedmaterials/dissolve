/*
	*** dUQ Methods - Twist
	*** src/lib/main/methods_twist.cpp
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
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/energykernel.h"
#include "classes/molecule.h"
#include "classes/changestore.h"
#include "base/comms.h"
#include "base/timer.h"

/*!
 * \brief Perform Torsional Twists
 */
CommandReturnValue DUQ::twist(Configuration& cfg)
{
	// Get method arguments from Command
	bool result;
	int nAttemptsPerBond = commandArgumentAsInteger("twist", "n", result);
	if (!result) return CommandFail;
	double cutoffSq = commandArgumentAsDouble("twist", "cut", result);
	if (!result) return CommandFail;
	if (cutoffSq < 0.0) cutoffSq = pairPotentialRange_;
	msg.print("Twist: Cutoff distance is %f\n", cutoffSq);
	msg.print("Twist: Performing %i twists per Bond\n", nAttemptsPerBond);
	cutoffSq *= cutoffSq;

	// Start a Timer
	Timer timer;

	double delta, currentEnergy, newEnergy;
	Vec3<double> vec, centre;
	int n, nAtoms, term, terminus, nTries = 0, nAccepted = 0, rootIndex;
	Matrix3 rotation;
	Atom** connectedAtoms;
	Bond* b;

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(DUQComm::World);

	// TODO This can be rewritten to calculate a local reference energy for each Grain in the Molecule and a Molecular inter-Grain energy.
	// Then, as atoms are selected and twisted all we need to calculate is the difference in energy for the Grains that have moved, and the
	// Molecular inter-Grain energy. Construct and maintain a RefList of Grains within the Molecule, with a boolean data to indicate Grain movement.
	
	// Loop over Molecules in sequence - energy will be calculated in parallel
	Comm.resetAccumulatedTime();
	timer.start();
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next)
	{
		// Add this Molecule's Grains to the ChangeStore
		changeStore.add(mol);

		// Calculate reference energy for the Molecule
		currentEnergy = kernel.energy(mol, DUQComm::World);

		// Grab the index of the first Atom in this Molecule
		rootIndex = mol->atom(0)->index();

		// Loop over Bonds
		RefList<Bond,int>& bonds = mol->bonds();
		for (term=0; term<bonds.nItems(); ++term)
		{
			b = bonds[term]->item;

			// Shift the fewest Atoms possible...
			terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
			nAtoms = b->nAttached(terminus);
			if (nAtoms < 2) continue;
			connectedAtoms = b->attached(terminus);
			++nTries;
			
			// Get bond vector
			if (cfg.box()->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) vec = cfg.box()->minimumVector(b->i(), b->j());
			else vec = b->j()->r() - b->i()->r();
			vec.normalise();

			rotation.createRotationAxis(vec.x, vec.y, vec.z, Comm.randomPlusMinusOne()*180.0, FALSE);
			for (n=0; n<nAtoms; ++n)
			{
				// Apply MIM to coordinates?
				if (cfg.box()->useMim(b->j()->grain()->cell(), connectedAtoms[n]->grain()->cell())) centre = cfg.box()->minimumImage(b->j(), connectedAtoms[n]);
				else centre = b->j()->r();
				connectedAtoms[n]->setCoordinates(rotation * (connectedAtoms[n]->r() - centre) + centre);
			}

			// Test energy again
			newEnergy = kernel.energy(mol, DUQComm::World);
			delta = newEnergy - currentEnergy;
			
			if ((delta < 0) || (Comm.random() < exp(-delta/(.008314472*temperature_))))
			{
// 				printf("Accepted move with current = %f, new = %f, delta = %f\n", currentEnergy, newEnergy, delta);
				changeStore.updateAtomsRelative(nAtoms, connectedAtoms, rootIndex);
				currentEnergy = newEnergy;
				++nAccepted;
			}
			else changeStore.revert();
		}

		// Serial method with parallel energy, so all processes already have new coordinates
		changeStore.reset();
	}
	timer.stop();

	// Distribute coordinate changes to all processes
// 	changeStore.distribute(cfg.nAtoms(), cfg.atoms());
// 	changeStore.reset();

	// Grains have moved, so refold and update locations
	updateGrains(cfg);

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return CommandCommFail;
	if (Comm.processGroupLeader())
	{
		double rate = double(nTries) / nAccepted;

		msg.print("Twist: Overall acceptance rate was %4.2f (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());

		// Adjust step size??

// 		translationStep_ /= ratio;
// 		rotationStep_ /= ratio;
// 		if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		msg.print("New steps = %f %f\n", translationStep_, rotationStep_);
	}

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	return CommandSuccess;
}
