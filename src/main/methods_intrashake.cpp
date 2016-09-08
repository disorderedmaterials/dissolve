/*
	*** dUQ Methods - IntraShake
	*** src/main/methods_intrashake.cpp
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
 * \brief Perform an Intramolecular Shake
 */
bool DUQ::intraShake(Configuration& cfg, int nShakesPerMol)
{
	// Start a Timer
	Timer timer;

	double distance, angle, delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Vec3<double> vec, vecji, vecjk, centre;
	int n, shake, nAttachedAtoms, terminus, nTries = 0, nAccepted = 0;
	Atom* i, *j, *k, *l;
	Matrix3 rotation;
	int* attachedIndices;
	const double rRT = 1.0/(.008314472*cfg.temperature());

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg, potentialMap_);

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
		for (shake = 0; shake<nShakesPerMol; ++shake)
		{
			// Loop over Bonds
			for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
			{
				if (b->interGrain()) continue;
				++nTries;

				// Grab pointers to atoms involved in bond
				i = mol->atom(b->indexI());
				j = mol->atom(b->indexJ());

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.useMim(i->grain()->cell(), j->grain()->cell())) vec = cfg.box()->minimumVector(i, j);
				else vec = j->r() - i->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				vec *= -delta * Comm.random();
				
				// Shift the fewest Atoms possible...
				// Need to take care and reverse the vector if necessary
				terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
				nAttachedAtoms = b->nAttached(terminus);
				attachedIndices = b->attachedIndices(terminus);
				if (terminus == 1) vec = -vec;
				for (n=0; n< nAttachedAtoms; ++n) mol->atom(attachedIndices[n])->translateCoordinates(vec);
			}
		}
		
		// Test energy again
		newEnergy = kernel.energy(mol, DUQComm::World);
		delta = newEnergy - currentEnergy;
		
		if ((delta < 0) || (Comm.random() < exp(-delta*rRT)))
		{
			changeStore.updateAll();
			currentEnergy = newEnergy;
			totalDelta += delta;
			++nAccepted;
		}
		else changeStore.revertAll();

		/*
		// Angles
		*/

		// Loop over nShakesPerMol
		for (shake = 0; shake<nShakesPerMol; ++shake)
		{
			// Loop over Angles
			for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
			{
				if (a->interGrain()) continue;
				++nTries;

				// Grab pointers to atoms involved in angle
				i = mol->atom(a->indexI());
				j = mol->atom(a->indexJ());
				k = mol->atom(a->indexK());

				// Get current angle and compare it to the equilibrium value
				// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
				if (cfg.useMim(j->grain()->cell(), i->grain()->cell())) vecji = cfg.box()->minimumVector(j, i);
				else vecji = i->r() - j->r();
				if (cfg.useMim(j->grain()->cell(), k->grain()->cell())) vecjk = cfg.box()->minimumVector(j, k);
				else vecjk = k->r() - j->r();
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
				nAttachedAtoms = a->nAttached(terminus);
				attachedIndices = a->attachedIndices(terminus);
				rotation.createRotationAxis(vec.x, vec.y, vec.z, terminus == 0 ? delta : -delta, false);
				for (n=0; n< nAttachedAtoms; ++n)
				{
					l = mol->atom(attachedIndices[n]);
					// Apply MIM to coordinates?
					if (cfg.useMim(j->grain()->cell(), l->grain()->cell())) centre = cfg.box()->minimumImage(j, l);
					else centre = j->r();
					l->setCoordinates(rotation * (l->r() - centre) + centre);
				}
			}

			// Test energy again
			newEnergy = kernel.energy(mol, DUQComm::World);
			delta = newEnergy - currentEnergy;
			
			if ((delta < 0) || (Comm.random() < exp(-delta*rRT)))
			{
				changeStore.updateAll();
				currentEnergy = newEnergy;
				totalDelta += delta;
				++nAccepted;
			}
			else changeStore.revertAll();
		}

		// Serial method with parallel energy, so all processes already have new coordinates
		changeStore.reset();
	}
	timer.stop();

// 	// Distribute coordinate changes to all processes
// 	changeStore.distributeAndApply(cfg.nAtoms(), cfg.atoms());
// 	changeStore.reset();

	// Grains have moved, so refold and update locations
	cfg.updateGrains();

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&totalDelta, 1, DUQComm::Leaders)) return false;
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
	if (nAccepted > 0) cfg.incrementCoordinateIndex();

	return true;
}

/*!
 * \brief Shake intermolecular terms between Grains
 */
bool DUQ::interShake(Configuration& cfg)
{
	// Initialise the Cell distributor
	const bool willBeModified = true, allowRepeats = false;
	cfg.initialiseCellDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg, potentialMap_);

	// Enter calculation loop until no more Cells are available
	int cellId, n;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double delta, currentGrainEnergy, newGrainEnergy, currentBondEnergy, newBondEnergy, distance;
	Cell* cell;
	SpeciesBond* b;
	Atom* localAtom, *otherAtom, *i, *j;
	Grain* grainI;
	Molecule* mol;
	Vec3<double> vec;
	const double rRT = 1.0/(.008314472*cfg.temperature());

	Timer timer;
	Comm.resetAccumulatedTime();
	while (cellId = cfg.nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distributeAndApply(cfg);
			cfg.finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg.cell(cellId);
		msg.printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nTotalCellNeighbours());

		/*
		 * Calculation Begins
		 */

		for (n=0; n<cell->nGrains(); ++n)
		{
			// Get current Grain and calculate base energy (inter-Grain energy with NO inter-Grain corrections)
			grainI = cell->grain(n);
			mol = grainI->parent();
			currentGrainEnergy = kernel.energy(grainI, cell->atoms(), EnergyKernel::ExcludeSelfFlag, DUQComm::Group);
			currentGrainEnergy += kernel.energy(grainI, cell->atomNeighbours(), EnergyKernel::NoFlags, DUQComm::Group);
			currentGrainEnergy += kernel.energy(grainI, cell->mimAtomNeighbours(), EnergyKernel::ApplyMinimumImage, DUQComm::Group);

			// Set current Grain as target in ChangeStore
			changeStore.add(grainI);

			// Loop over Bond connections
			for (RefListItem<SpeciesBond,int>* refBond = grainI->source()->bondConnections(); refBond != NULL; refBond = refBond->next)
			{
				b = refBond->item;
				
				// Grab pointers to the atoms involved in the bond
				i = mol->atom(b->indexI());
				j = mol->atom(b->indexJ());

				// Determine local Atom to this Grain
				if (i->grain() == grainI)
				{
					localAtom = i;
					otherAtom = j;
				}
				else
				{
					localAtom = j;
					otherAtom = i;
				}
				
				// We will always move *towards* the higher index Grain, so if the Atom index on the other atom is *lower*, continue the loop.
				if (otherAtom->index() < localAtom->index()) continue;

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.useMim(cell, otherAtom->grain()->cell())) vec = cfg.box()->minimumVector(localAtom, otherAtom);
				else vec = otherAtom->r() - localAtom->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				// TODO Implement DUQComm random buffer here!
				currentBondEnergy = kernel.energy(mol, b);
				vec *= delta * DUQMath::random();
				grainI->translate(vec);

				// Calculate new energy
				newGrainEnergy = kernel.energy(grainI, cell->atoms(), EnergyKernel::ExcludeSelfFlag, DUQComm::Group);
				newGrainEnergy += kernel.energy(grainI, cell->atomNeighbours(), EnergyKernel::NoFlags, DUQComm::Group);
				newGrainEnergy += kernel.energy(grainI, cell->mimAtomNeighbours(), EnergyKernel::ApplyMinimumImage, DUQComm::Group);
				newBondEnergy = kernel.energy(mol, b);

				// Trial the transformed Grain position (the Master is in charge of this)
				delta = (newGrainEnergy + newBondEnergy) - (currentGrainEnergy + currentBondEnergy);
				accept = delta < 0 ? true : (DUQMath::random() < exp(-delta*rRT));

				// Broadcast result to process group
				if (!Comm.broadcast(&accept, 1, 0, DUQComm::Group)) return false;
				if (accept)
				{
// 					msg.print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Grain
					changeStore.updateAll();
					currentGrainEnergy = newGrainEnergy;
					++nAccepted;
				}
				else changeStore.revertAll();
			}
			
			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();
		}

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distributeAndApply(cfg);
		changeStore.reset();

		// Must unlock the Cell when we are done with it!
		cfg.finishedWithCell(willBeModified, cellId);
	}
	timer.stop();

	// Grains have moved, so refold and update locations
	cfg.updateGrains();

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return false;
	if (Comm.processGroupLeader())
	{
		msg.print("InterShake: Overall acceptance rate was %6.1f%% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*nAccepted / nTries, nAccepted, nTries, timer.timeString(), Comm.accumulatedTimeString());

// 		// Adjust step size?
// 		double acceptanceRate = double(nAccepted)/nTries;
// 		if (acceptanceRate < reqAcceptandRate)
// 		{
// 			translationStep_ *= 1.0-(0.1 * DUQMath::random());
// 			rotationStep_ *= 1.0-(0.1 * DUQMath::random());
// 		}
// 		else
// 		{
// 			translationStep_ /= 1.0-(0.1 * DUQMath::random());
// 			rotationStep_ /= 1.0-(0.1 * DUQMath::random());
// 		}
// 		if (translationStep_ < 0.05) translationStep_ = 0.05;
// 		else if (translationStep_ > maxTranslationStep_) translationStep_ = maxTranslationStep_;
// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
// 		msg.print("New steps = %f %f\n", translationStep_, rotationStep_);
	}

	// Increment configuration changeCount_
	if (nAccepted > 0) cfg.incrementCoordinateIndex();

	return true;
}

/*!
 * \brief Individually Shake all Intramolecular Terms
 */
bool DUQ::termShake(Configuration& cfg, int nShakesPerTerm)
{
	// Start a Timer
	Timer timer;

	double distance, angle, delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Vec3<double> vec, vecji, vecjk, centre;
	int n, shake, nAttachedAtoms, terminus, nTries = 0, nAccepted = 0, rootIndex;
	Matrix3 rotation;
	int* attachedIndices;
	Atom* i, *j, *k, *l;
	const double rRT = 1.0/(.008314472*cfg.temperature());

	// Create an EnergyKernel
	EnergyKernel kernel(cfg, potentialMap_);

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
		currentEnergy = kernel.energy(mol);

		// Grab the index of the first Atom in this Molecule
		rootIndex = mol->atom(0)->index();

		/*
		// Bonds
		*/

		// Loop over nShakesPerMol
		for (shake = 0; shake<nShakesPerTerm; ++shake)
		{
			// Loop over Bonds
			for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
			{
				++nTries;

				// Grab pointers to atoms involved in bond
				i = mol->atom(b->indexI());
				j = mol->atom(b->indexJ());

				// Get current distance of this Bond, and compare it to the equilibrium distance
				if (cfg.useMim(i->grain()->cell(), j->grain()->cell())) vec = cfg.box()->minimumVector(i, j);
				else vec = j->r() - i->r();
				distance = vec.magAndNormalise();
				delta = b->equilibrium() - distance;
				
				// The delta now reflects the distance and direction we should try to travel.
				vec *= -delta * DUQMath::random();
				
				// Shift the fewest Atoms possible...
				// Need to take care and reverse the vector if necessary
				terminus = b->nAttached(0) > b->nAttached(1) ? 1 : 0;
				nAttachedAtoms = b->nAttached(terminus);
				attachedIndices = b->attachedIndices(terminus);
				if (terminus == 1) vec = -vec;
				for (n=0; n<nAttachedAtoms; ++n) mol->atom(attachedIndices[n])->translateCoordinates(vec);
// 				if (count == 9) printf("Changing Bond %i-%i from %f to %f\n", b->i()->index(), b->j()->index(), distance, cfg.box()->minimumDistance(b->i(), b->j()));
		
				// Test energy again
				newEnergy = kernel.energy(mol);
				delta = newEnergy - currentEnergy;
				
				if ((delta < 0) || (DUQMath::random() < exp(-delta*rRT)))
				{
					changeStore.updateAtomsLocal(nAttachedAtoms, attachedIndices);
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revertAll();
			}
		}

		/*
		// Angles
		*/

		// Loop over nShakesPerMol
		for (shake = 0; shake<nShakesPerTerm; ++shake)
		{
			// Loop over angles
			for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
			{
				++nTries;

				// Grab pointers to atoms involved in bond
				i = mol->atom(a->indexI());
				j = mol->atom(a->indexJ());
				k = mol->atom(a->indexK());

				// Get current angle and compare it to the equilibrium value
				// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
				if (cfg.useMim(j->grain()->cell(), i->grain()->cell())) vecji = cfg.box()->minimumVector(j, i);
				else vecji = i->r() - j->r();
				if (cfg.useMim(j->grain()->cell(), k->grain()->cell())) vecjk = cfg.box()->minimumVector(j, k);
				else vecjk = k->r() - j->r();
				angle = Box::angle(vecji, vecjk);
				delta = a->equilibrium() - angle;
				
				// The delta now reflects the distance and direction we should try to travel.
				delta *= DUQMath::random();
				vec = vecji*vecjk;

				// Shift the fewest Atoms possible...
				// Again, take care to rotate in the correct direction depending on which terminus we select
				terminus = a->nAttached(0) > a->nAttached(1) ? 1 : 0;
				nAttachedAtoms = a->nAttached(terminus);
				attachedIndices = a->attachedIndices(terminus);
				rotation.createRotationAxis(vec.x, vec.y, vec.z, terminus == 0 ? delta : -delta, false);
				for (n=0; n< nAttachedAtoms; ++n)
				{
					l = mol->atom(attachedIndices[n]);
					// Apply MIM to coordinates?
					if (cfg.useMim(j->grain()->cell(), l->grain()->cell())) centre = cfg.box()->minimumImage(j, l);
					else centre = j->r();
					l->setCoordinates(rotation * (l->r() - centre) + centre);
				}
				// TEST
// 				if (cfg.useMim(j->grain()->cell(), i->grain()->cell())) vecji = cfg.box()->minimumVector(j, i);
// 				else vecji = i->r() - j->r();
// 				if (cfg.useMim(j->grain()->cell(), k->grain()->cell())) vecjk = cfg.box()->minimumVector(j, k);
// 				else vecjk = k->r() - j->r();
// 				vecji.normalise();
// 				vecjk.normalise();
// 				if (count == 9) printf("Changed Angle %i-%i-%i from %f to %f.\n", a->i()->index(), a->j()->index(), a->k()->index(), angle, Box::angle(vecji, vecjk));

				// Test energy again
				newEnergy = kernel.energy(mol);
				delta = newEnergy - currentEnergy;
				
				if ((delta < 0) || (DUQMath::random() < exp(-delta*rRT)))
				{
					changeStore.updateAtomsLocal(nAttachedAtoms, attachedIndices);
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revertAll();
			}
		}

		// Store modifications to this Molecule's Atom positions ready for broadcast later
		changeStore.storeAndReset();
	}
	timer.stop();

	// Store modifications to Atom positions ready for broadcast later
	changeStore.storeAndReset();

	// Distribute coordinate changes to all processes
	changeStore.distributeAndApply(cfg);
	changeStore.reset();

	// Grains have moved, so refold and update locations
	cfg.updateGrains();

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&nTries, 1, DUQComm::Leaders)) return false;
	if (!Comm.allSum(&totalDelta, 1, DUQComm::Leaders)) return false;
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
	if (nAccepted > 0) cfg.incrementCoordinateIndex();

	return true;
}
