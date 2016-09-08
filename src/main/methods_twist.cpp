/*
	*** dUQ Methods - Twist
	*** src/main/methods_twist.cpp
	Copyright T. Youngs 2012-2014

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
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/comms.h"
#include "base/timer.h"

/*!
 * \brief Perform Torsional Twists
 */
bool DUQ::twist(Configuration& cfg, double cutoffDistance, int nTwistsPerTerm)
{
	// Check method arguments if necessary
	if (cutoffDistance < 0.0) cutoffDistance = pairPotentialRange_;
	const double cutoffSq = cutoffDistance * cutoffDistance;
	const double rRT = 1.0/(.008314472*cfg.temperature());

	msg.print("Twist: Cutoff distance is %f\n", cutoffDistance);
	msg.print("Twist: Performing %i twists per Bond\n", nTwistsPerTerm);

	// Start a Timer
	Timer timer;

	double delta, currentEnergy, newEnergy;
	Vec3<double> vec, centre;
	int n, nAttachedAtoms, terminus, nTries = 0, nAccepted = 0, rootIndex;
	Matrix3 rotation;
	int* attachedIndices;
	Atom* i, *j, *l;

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg, potentialMap_);

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(DUQComm::World);

	// TODO This can be rewritten to calculate a local reference energy for each Grain in the Molecule and a Molecular inter-Grain energy.
	// Then, as atoms are selected and twisted all we need to calculate is the difference in energy for the Grains that have moved, and the
	// Molecular inter-Grain energy. Construct and maintain a RefList of Grains within the Molecule, with a boolean datum to indicate Grain movement.
	
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
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			// Grab atom pointers involved in the bond
			i = mol->atom(b->indexI());
			j = mol->atom(b->indexJ());

			// Shift the fewest Atoms possible...
			terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
			nAttachedAtoms = b->nAttached(terminus);
			if (nAttachedAtoms < 2) continue;
			attachedIndices = b->attachedIndices(terminus);
			++nTries;
			
			// Get bond vector
			if (cfg.useMim(i->grain()->cell(), j->grain()->cell())) vec = cfg.box()->minimumVector(i, j);
			else vec = j->r() - i->r();
			vec.normalise();

			rotation.createRotationAxis(vec.x, vec.y, vec.z, Comm.randomPlusMinusOne()*180.0, false);
			for (n=0; n< nAttachedAtoms; ++n)
			{
				l = mol->atom(attachedIndices[n]);

				// Apply MIM to coordinates?
				if (cfg.useMim(j->grain()->cell(), l->grain()->cell())) centre = cfg.box()->minimumImage(j, l);
				else centre = j->r();
				l->setCoordinates(rotation * (l->r() - centre) + centre);
			}

			// Test energy again
			newEnergy = kernel.energy(mol, DUQComm::World);
			delta = newEnergy - currentEnergy;
			
			if ((delta < 0) || (Comm.random() < exp(-delta*rRT)))
			{
// 				printf("Accepted move with current = %f, new = %f, delta = %f\n", currentEnergy, newEnergy, delta);
				changeStore.updateAtomsLocal(nAttachedAtoms, attachedIndices);
				currentEnergy = newEnergy;
				++nAccepted;
			}
			else changeStore.revertAll();
		}

		// Serial method with parallel energy, so all processes already have new coordinates
		changeStore.reset();
	}
	timer.stop();

	// Distribute coordinate changes to all processes
// 	changeStore.distributeAndApply(cfg.nAtoms(), cfg.atoms());
// 	changeStore.reset();

	// Grains have moved, so refold and update locations
	cfg.updateGrains();

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return false;
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
	if (nAccepted > 0) cfg.incrementCoordinateIndex();

	return true;
}
