/*
	*** dUQ Methods - Upkeep
	*** src/main/methods_upkeep.cpp
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
#include "classes/grain.h"
#include "classes/changestore.h"

/*!
 * \brief Update Grains
 */
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

/*!
 * \brief Recalculate cell atom neighbour lists
 */
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
	msg.print("--> Cutoff for atom cell neighbours is %f\n", cutoffSq);
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
	msg.print("--> Cell atom neighbour lists generated (%s).\n", timer.timeString());
}