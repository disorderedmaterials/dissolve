/*
	*** dUQ Methods - IntraShake
	*** src/lib/main/methods_intrashake.cpp
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
 * \brief Perform an Intramolecular Shake
 */
CommandReturnValue DUQ::intraShake(Configuration& cfg, int nShakesPerMol)
{
	// Start a Timer
	Timer timer;

	double distance, angle, delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Vec3<double> vec, vecji, vecjk, centre;
	int n, shake, nAtoms, term, terminus, nTries = 0, nAccepted = 0;
	Matrix3 rotation;
	Atom** connectedAtoms;
	Angle* a;
	Bond* b;

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(DUQComm::World);

	// Loop over Molecules
	Comm.resetAccumulatedTime();
	timer.start();
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next)
	{
		// Add this Molecule's Grains to the ChangeStore
		changeStore.add(mol);

		// Calculate reference energy for the Molecule
		currentEnergy = kernel.energy(mol, DUQComm::World);

		/*
		// Bonds
		*/

		// Loop over nShakesPerMol
		RefList<Bond,int>& bonds = mol->bonds();
		for (shake = 0; shake<nShakesPerMol; ++shake)
		{
			// Loop over Bonds
			for (term=0; term<bonds.nItems(); ++term)
			{
				b = bonds[term]->item;
				if (b->interGrain()) continue;
				++nTries;

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.box()->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) vec = cfg.box()->minimumVector(b->i(), b->j());
				else vec = b->j()->r() - b->i()->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				vec *= -delta * Comm.random();
				
				// Shift the fewest Atoms possible...
				// Need to take care and reverse the vector if necessary
				terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
				nAtoms = b->nAttached(terminus);
				connectedAtoms = b->attached(terminus);
				if (terminus == 1) vec = -vec;
				for (n=0; n<nAtoms; ++n) connectedAtoms[n]->translateCoordinates(vec);
			}
		}
		
		// Test energy again
		newEnergy = kernel.energy(mol, DUQComm::World);
		delta = newEnergy - currentEnergy;
		
		if ((delta < 0) || (Comm.random() < exp(-delta/(.008314472*temperature_))))
		{
			changeStore.updateAll();
			currentEnergy = newEnergy;
			totalDelta += delta;
			++nAccepted;
		}
		else changeStore.revert();

		/*
		// Angles
		*/

		// Loop over nShakesPerMol
		RefList<Angle,int>& angles = mol->angles();
		for (shake = 0; shake<nShakesPerMol; ++shake)
		{
			// Loop over Angles
			for (term=0; term<angles.nItems(); ++term)
			{
				a = angles[term]->item;
				if (a->interGrain()) continue;
				++nTries;

				// Get current angle and compare it to the equilibrium value
				// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->i()->grain()->cell())) vecji = cfg.box()->minimumVector(a->j(), a->i());
				else vecji = a->i()->r() - a->j()->r();
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->k()->grain()->cell())) vecjk = cfg.box()->minimumVector(a->j(), a->k());
				else vecjk = a->k()->r() - a->j()->r();
				vecji.normalise();
				vecjk.normalise();
				angle = Box::angle(vecji, vecjk);
				delta = a->equilibrium() - angle;
				
				// The delta now reflects the distance and direction we should try to travel.
				delta *= Comm.random();
				vec = vecji*vecjk;

				// Shift the fewest Atoms possible...
				// Again, take care to rotate in the correct direction depending on which terminus we select
				terminus = a->nAttached(0) > a->nAttached(1) ? 1 : 0;
				nAtoms = a->nAttached(terminus);
				connectedAtoms = a->attached(terminus);
				rotation.createRotationAxis(vec.x, vec.y, vec.z, terminus == 0 ? delta : -delta, FALSE);
				for (n=0; n<nAtoms; ++n)
				{
					// Apply MIM to coordinates?
					if (cfg.box()->useMim(a->j()->grain()->cell(), connectedAtoms[n]->grain()->cell())) centre = cfg.box()->minimumImage(a->j(), connectedAtoms[n]);
					else centre = a->j()->r();
					connectedAtoms[n]->setCoordinates(rotation * (connectedAtoms[n]->r() - centre) + centre);
				}
			}

			// Test energy again
			newEnergy = kernel.energy(mol, DUQComm::World);
			delta = newEnergy - currentEnergy;
			
			if ((delta < 0) || (Comm.random() < exp(-delta/(.008314472*temperature_))))
			{
				changeStore.updateAll();
				currentEnergy = newEnergy;
				totalDelta += delta;
				++nAccepted;
			}
			else changeStore.revert();
		}

		// Serial method with parallel energy, so all processes already have new coordinates
		changeStore.reset();
	}
	timer.stop();

// 	// Distribute coordinate changes to all processes
// 	changeStore.distribute(cfg.nAtoms(), cfg.atoms());
// 	changeStore.reset();

	// Grains have moved, so refold and update locations
	updateGrains(cfg);

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&totalDelta, 1, DUQComm::Leaders)) return CommandCommFail;
	if (Comm.processGroupLeader())
	{
		msg.print("IntraShake: Overall acceptance rate was %6.1f%% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*nAccepted / nTries, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());

		// Adjust step size??
// 		double ratio = 0.3 / (double(nAccepted)/nTries);
// 		translationStep_ /= ratio;
// 		rotationStep_ /= ratio;
// 		if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		msg.print("New steps = %f %f\n", translationStep_, rotationStep_);
	}

	// Update total energy
	registerEnergyChange(totalDelta);
	accumulateEnergyChange();

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	return CommandSuccess;
}

/*!
 * \brief Shake intermolecular terms between Grains
 */
CommandReturnValue DUQ::interShake(Configuration& cfg)
{
	// Initialise the Cell distributor
	const bool willBeModified = TRUE, allowRepeats = FALSE;
	cfg.initialiseCellDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Enter calculation loop until no more Cells are available
	int cellId, n;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double delta, currentGrainEnergy, newGrainEnergy, currentBondEnergy, newBondEnergy, distance;
	Cell* cell;
	Bond* b;
	Atom* localAtom, *otherAtom;
	Grain* grainI;
	Vec3<double> vec;

	Timer timer;
	Comm.resetAccumulatedTime();
	while (cellId = cfg.nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distribute(cfg.nAtoms(), cfg.atoms());
			cfg.finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg.box()->cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nNeighbours());

		/*
		 * Calculation Begins
		 */

		for (n=0; n<cell->nGrains(); ++n)
		{
			// Get current Grain and calculate base energy (inter-Grain energy with NO inter-Grain corrections)
			grainI = cell->grain(n);
			currentGrainEnergy = kernel.energy(grainI, cell->neighbours(), FALSE, DUQComm::Group);
			currentGrainEnergy += kernel.energy(grainI, cell, FALSE, FALSE, DUQComm::Group);

			// Set current Grain as target in ChangeStore
			changeStore.add(grainI);

			// Loop over Bond connections
			for (RefListItem<Bond,int>* refBond = grainI->bondConnections(); refBond != NULL; refBond = refBond->next)
			{
				b = refBond->item;

				// Determine local Atom to this Grain
				if (b->i()->grain() == grainI)
				{
					localAtom = b->i();
					otherAtom = b->j();
				}
				else
				{
					localAtom = b->j();
					otherAtom = b->i();
				}
				
				// We will always move *towards* the higher index Grain, so if the Atom index on the other atom is *lower*, continue the loop.
				if (otherAtom->index() < localAtom->index()) continue;

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.box()->useMim(cell, otherAtom->grain()->cell())) vec = cfg.box()->minimumVector(localAtom, otherAtom);
				else vec = otherAtom->r() - localAtom->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				// TODO To avoid constant sending of single values because of randomness, implement dUQMath::randomBuffer() which stores a chunk of random numbers
				currentBondEnergy = kernel.energy(b);
				vec *= delta * dUQMath::random();
				grainI->translate(vec);

				// Calculate new energy
				newGrainEnergy = kernel.energy(grainI, cell->neighbours(), FALSE, DUQComm::Group);
				newGrainEnergy += kernel.energy(grainI, cell, FALSE, FALSE, DUQComm::Group);
				newBondEnergy = kernel.energy(b);

				// Trial the transformed Grain position (the Master is in charge of this)
				delta = (newGrainEnergy + newBondEnergy) - (currentGrainEnergy + currentBondEnergy);
				accept = delta < 0 ? TRUE : (dUQMath::random() < exp(-delta/(.008314472*temperature_)));

				// Broadcast result to process group
				if (!Comm.broadcast(&accept, 1, 0, DUQComm::Group)) return CommandCommFail;
				if (accept)
				{
// 					msg.print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Grain
					changeStore.updateAll();
					currentGrainEnergy = newGrainEnergy;
					++nAccepted;
				}
				else changeStore.revert();
			}
			
			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();
		}

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distribute(cfg.nAtoms(), cfg.atoms());
		changeStore.reset();

		// Must unlock the Cell when we are done with it!
		cfg.finishedWithCell(willBeModified, cellId);
	}
	timer.stop();

	// Grains have moved, so refold and update locations
	updateGrains(cfg);

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return CommandCommFail;
	if (Comm.processGroupLeader())
	{
		msg.print("InterShake: Overall acceptance rate was %6.1f%% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*nAccepted / nTries, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());

// 		// Adjust step size?
// 		double acceptanceRate = double(nAccepted)/nTries;
// 		if (acceptanceRate < reqAcceptandRate)
// 		{
// 			translationStep_ *= 1.0-(0.1 * dUQMath::random());
// 			rotationStep_ *= 1.0-(0.1 * dUQMath::random());
// 		}
// 		else
// 		{
// 			translationStep_ /= 1.0-(0.1 * dUQMath::random());
// 			rotationStep_ /= 1.0-(0.1 * dUQMath::random());
// 		}
// 		if (translationStep_ < 0.05) translationStep_ = 0.05;
// 		else if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
// 		msg.print("New steps = %f %f\n", translationStep_, rotationStep_);
	}

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	return CommandSuccess;
}

/*!
 * \brief Individually Shake all Intramolecular Terms
 */
CommandReturnValue DUQ::termShake(Configuration& cfg, int nShakesPerTerm)
{
	// Start a Timer
	Timer timer;

	double distance, angle, delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Vec3<double> vec, vecji, vecjk, centre;
	int n, shake, nAtoms, term, terminus, nTries = 0, nAccepted = 0, rootIndex;
	Matrix3 rotation;
	Atom** connectedAtoms;
	Angle* a;
	Bond* b;

	// Create an EnergyKernel
	EnergyKernel kernel(cfg.box(), potentialMap_);

	// Create a changestore
	ChangeStore changeStore;
	
	// Loop over Molecules	TODO Split with MPI
	Comm.resetAccumulatedTime();
	timer.start();
	for (Molecule* mol = cfg.molecules(); mol != NULL; mol = mol->next)
	{
		// Add this Molecule's Grains to the ChangeStore
		changeStore.add(mol);

		// Calculate reference energy for the Molecule
		currentEnergy = kernel.energy(mol, pairPotentialRangeSquared_);

		// Grab the index of the first Atom in this Molecule
		rootIndex = mol->atom(0)->index();

		/*
		// Bonds
		*/

		// Loop over nShakesPerMol
		RefList<Bond,int>& bonds = mol->bonds();
		for (shake = 0; shake<nShakesPerTerm; ++shake)
		{
			// Loop over Bonds
			for (term=0; term<bonds.nItems(); ++term)
			{
				b = bonds[term]->item;
				++nTries;

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.box()->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) vec = cfg.box()->minimumVector(b->i(), b->j());
				else vec = b->j()->r() - b->i()->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				vec *= -delta * dUQMath::random();
				
				// Shift the fewest Atoms possible...
				// Need to take care and reverse the vector if necessary
				terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
				nAtoms = b->nAttached(terminus);
				connectedAtoms = b->attached(terminus);
				if (terminus == 1) vec = -vec;
				for (n=0; n<nAtoms; ++n) connectedAtoms[n]->translateCoordinates(vec);
// 				if (count == 9) printf("Changing Bond %i-%i from %f to %f\n", b->i()->index(), b->j()->index(), distance, cfg.box()->minimumDistance(b->i(), b->j()));
		
				// Test energy again
				newEnergy = kernel.energy(mol, pairPotentialRangeSquared_);
				delta = newEnergy - currentEnergy;
				
				if ((delta < 0) || (dUQMath::random() < exp(-delta/(.008314472*temperature_))))
				{
					changeStore.updateAtomsRelative(nAtoms, connectedAtoms, rootIndex);
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revert();
			}
		}

		/*
		// Angles
		*/

		// Loop over nShakesPerMol
		RefList<Angle,int>& angles = mol->angles();
		for (shake = 0; shake<nShakesPerTerm; ++shake)
		{
			// Loop over Angles
			for (term=0; term<angles.nItems(); ++term)
			{
				a = angles[term]->item;
				++nTries;

				// Get current angle and compare it to the equilibrium value
				// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->i()->grain()->cell())) vecji = cfg.box()->minimumVector(a->j(), a->i());
				else vecji = a->i()->r() - a->j()->r();
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->k()->grain()->cell())) vecjk = cfg.box()->minimumVector(a->j(), a->k());
				else vecjk = a->k()->r() - a->j()->r();
				angle = Box::angle(vecji, vecjk);
				delta = a->equilibrium() - angle;
				
				// The delta now reflects the distance and direction we should try to travel.
				delta *= dUQMath::random();
				vec = vecji*vecjk;

				// Shift the fewest Atoms possible...
				// Again, take care to rotate in the correct direction depending on which terminus we select
				terminus = a->nAttached(0) > a->nAttached(1) ? 1 : 0;
				nAtoms = a->nAttached(terminus);
				connectedAtoms = a->attached(terminus);
				rotation.createRotationAxis(vec.x, vec.y, vec.z, terminus == 0 ? delta : -delta, FALSE);
				for (n=0; n<nAtoms; ++n)
				{
					// Apply MIM to coordinates?
					if (cfg.box()->useMim(a->j()->grain()->cell(), connectedAtoms[n]->grain()->cell())) centre = cfg.box()->minimumImage(a->j(), connectedAtoms[n]);
					else centre = a->j()->r();
					connectedAtoms[n]->setCoordinates(rotation * (connectedAtoms[n]->r() - centre) + centre);
				}
				// TEST
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->i()->grain()->cell())) vecji = cfg.box()->minimumVector(a->j(), a->i());
				else vecji = a->i()->r() - a->j()->r();
				if (cfg.box()->useMim(a->j()->grain()->cell(), a->k()->grain()->cell())) vecjk = cfg.box()->minimumVector(a->j(), a->k());
				else vecjk = a->k()->r() - a->j()->r();
				vecji.normalise();
				vecjk.normalise();
// 				if (count == 9) printf("Changed Angle %i-%i-%i from %f to %f.\n", a->i()->index(), a->j()->index(), a->k()->index(), angle, Box::angle(vecji, vecjk));

				// Test energy again
				newEnergy = kernel.energy(mol, pairPotentialRangeSquared_);
				delta = newEnergy - currentEnergy;
				
				if ((delta < 0) || (dUQMath::random() < exp(-delta/(.008314472*temperature_))))
				{
					changeStore.updateAtomsRelative(nAtoms, connectedAtoms, rootIndex);
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revert();
			}
		}

		// Store modifications to this Molecule's Atom positions ready for broadcast later
		changeStore.storeAndReset();
	}
	timer.stop();

	// Store modifications to Atom positions ready for broadcast later
	changeStore.storeAndReset();

	// Distribute coordinate changes to all processes
	changeStore.distribute(cfg.nAtoms(), cfg.atoms());
	changeStore.reset();

	// Grains have moved, so refold and update locations
	updateGrains(cfg);

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return CommandCommFail;
	if (!Comm.allSum(&totalDelta, 1, DUQComm::Leaders)) return CommandCommFail;
	if (Comm.processGroupLeader())
	{
		msg.print("TermShake: Overall acceptance rate was %6.1f%% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*nAccepted / nTries, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());

		// Adjust step size??
// 		double ratio = 0.3 / (double(nAccepted)/nTries);
// 		translationStep_ /= ratio;
// 		rotationStep_ /= ratio;
// 		if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		msg.print("New steps = %f %f\n", translationStep_, rotationStep_);
	}

	// Update total energy
	registerEnergyChange(totalDelta);
	accumulateEnergyChange();

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementChangeCount();

	return CommandSuccess;
}
