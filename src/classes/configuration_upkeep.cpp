/*
	*** dUQ Methods - Upkeep
	*** src/main/methods_upkeep.cpp
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

#include "main/duq.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/changestore.h"

// Update cell locations of all atoms
bool Configuration::updateAtomsInCells()
{
	// Fold the coordinates of each atom into the box, and then check its cell location, moving if necessary.
	// Loop over cells, focussing on one atom index at a time. Do it this way to try and avoid moving large numbers of atoms into one cell at once, causing an overflow.
	int n, atomId = 0;
	Cell* currentCell, *targetCell;
	Vec3<double> foldedR;
	Atom* i;
	for (int c = 0; c < nCells_; ++c)
	{
		// Grab cell pointer
		currentCell = &cells_[c];

		// TODO Overload cell() to take a pointer to a Vec3<> in which the folded r can be returned
		for (OrderedPointerListItem<Atom>* item = currentCell->atoms().first(); item != NULL; item = item->next)
		{
			i = item->object();
			foldedR = box_->fold(i->r());
			i->setCoordinates(foldedR);
			targetCell = cell(i->r());

			// Need to move?
			if (targetCell != currentCell)
			{
				// We first must remove
				if (!currentCell->moveAtom(i, targetCell)) return false;
			}
		}
	}

	return true;
}

// Update cell locations of specified atom index
bool Configuration::updateAtomInCell(int id)
{
	// Fold the coordinates of specified atom into the box, and then check its cell location, moving if necessary.
	Cell* currentCell, *targetCell;
	Vec3<double> foldedR;

	// Grab atom pointers
	Atom* i = &atoms_[id];

	// TODO Overload cell() to take a pointer to a Vec3<> in which the folded r can be returned
	// Grab current cell pointer, and calculate folded coordinate and new cell location
	currentCell = i->cell();
	foldedR = box_->fold(i->r());
	i->setCoordinates(foldedR);
	targetCell = cell(foldedR);

	// Need to move?
	if (targetCell != currentCell) currentCell->moveAtom(i, targetCell);

	return true;
}

// Update Grains
void Configuration::updateGrains()
{
	// Loop over Grains, folding them into the Box and reassigning Cell locations
	Cell* inCell;
	Vec3<double> newCentre;
	for (int n=0; n<nGrains_; ++n)
	{
		newCentre = box_->fold(grains_[n].centre());
		grains_[n].moveTo(newCentre);
		inCell = cell(newCentre);
		
		// Is the Grain now in a different Cell?
		if (inCell != grains_[n].cell())
		{
			if (grains_[n].cell() != NULL) grains_[n].removeFromCell(NULL);
			inCell->addGrain(&grains_[n]);
		}
	}
}

// Recalculate cell atom neighbour lists
void Configuration::recreateCellAtomNeighbourLists(double pairPotentialRange)
{
	// Clear all existing atoms in cell neighbour lists
	for (int n=0; n<nCells_; ++n) cell(n)->clearAtomNeighbourList();

	Vec3<double> r, atomR;
	double distSq, cutoffSq;
	Atom** otherAtoms;
	Atom* i;
	Cell* centralCell;
	Cell** neighbours, **mimNeighbours;
	int nNeighbours, nMimNeighbours, c;

	// Calculate cutoff distance squared
	cutoffSq = pairPotentialRange + sqrt(realCellSize_.dp(realCellSize_))*0.5;
	Messenger::print("--> Cutoff for atom cell neighbours is %f\n", cutoffSq);
	cutoffSq *= cutoffSq;

	// Loop over atoms
	Timer timer;
	for (int n=0; n<nAtoms_; ++n)
	{
		// Grab reference to atom and pointer to its current cell location
		i = &atoms_[n];
		centralCell = i->cell();
		atomR = i->r();

		// Check distance of the current atom from the centres of each of the neighbouring cells
		neighbours = centralCell->cellNeighbours();
		nNeighbours = centralCell->nCellNeighbours();
		for (c=0; c<nNeighbours; ++c)
		{
			r = box_->minimumVector(neighbours[c]->centre(), atomR);
			distSq = r.magnitudeSq();
			if (distSq <= cutoffSq) neighbours[c]->addAtomToNeighbourList(i, false, true);
		}

		mimNeighbours = centralCell->mimCellNeighbours();
		nMimNeighbours = centralCell->nMimCellNeighbours();
		for (c=0; c<nMimNeighbours; ++c)
		{
			r = box_->minimumVector(mimNeighbours[c]->centre(), atomR);
			distSq = r.magnitudeSq();
			if (distSq <= cutoffSq) mimNeighbours[c]->addAtomToNeighbourList(i, true, true);
		}
	}
	timer.stop();
	Messenger::print("--> Cell atom neighbour lists generated (%s).\n", timer.totalTimeString());
}
