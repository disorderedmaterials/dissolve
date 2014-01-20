/*
	*** dUQ Methods - Upkeep
	*** src/lib/main/methods_upkeep.cpp
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
 * \brief Upkeep - Update Grains
 */
void DUQ::updateGrains(Configuration& cfg)
{
	// Loop over Grains, folding them into the Box and reassigning Cell locations
	Cell* cell;
	Vec3<double> newCentre;
	Grain* grains = cfg.grains();
	for (int n=0; n<cfg.nGrains(); ++n)
	{
		newCentre = cfg.box()->fold(grains[n].centre());
		grains[n].moveTo(newCentre);
		cell = cfg.cell(newCentre);
		
		// Is the Grain now in a different Cell?
		if (cell != grains[n].cell())
		{
			if (grains[n].cell() != NULL) grains[n].removeFromCell(NULL);
			cell->addGrain(&grains[n]);
		}
	}
}

/*!
 * \brief Upkeep - Recalculate cell atom neighbour lists
 */
void DUQ::createCellAtomNeighbourLists(Configuration& cfg)
{
	// Clear all existing atoms in cell neighbour lists
	for (int n=0; n<cfg.nCells(); ++n) cfg.cell(n)->clearAtomNeighbourList();

	Vec3<double> r, atomR;
	double distSq, cutoffSq = pairPotentialRange_ + sqrt(cfg.realCellSize().dp(cfg.realCellSize()))*0.5;
	msg.print("--> Cutoff for atom cell neighbours is %f\n", cutoffSq);
	cutoffSq *= cutoffSq;
	Atom** otherAtoms;
	Atom* i, *atoms = cfg.atoms();
	Cell* centralCell;
	Cell** neighbours, **mimNeighbours;
	int nNeighbours, nMimNeighbours, c;
	const Box* box = cfg.box();

	// Loop over atoms
	Timer timer;
	for (int n=0; n<cfg.nAtoms(); ++n)
	{
		// Grab reference to atom and pointer to its current cell location
		i = &atoms[n];
		centralCell = i->cell();
		atomR = i->r();

		// Check distance of the current atom from the centres of each of the neighbouring cells
		neighbours = centralCell->cellNeighbours();
		nNeighbours = centralCell->nCellNeighbours();
		for (c=0; c<nNeighbours; ++c)
		{
			r = box->minimumVector(neighbours[c]->centre(), atomR);
			distSq = r.magnitudeSq();
			if (distSq <= cutoffSq) neighbours[c]->addAtomToNeighbourList(i, false, true);
		}

		mimNeighbours = centralCell->mimCellNeighbours();
		nMimNeighbours = centralCell->nMimCellNeighbours();
		for (c=0; c<nMimNeighbours; ++c)
		{
			r = box->minimumVector(mimNeighbours[c]->centre(), atomR);
			distSq = r.magnitudeSq();
			if (distSq <= cutoffSq) mimNeighbours[c]->addAtomToNeighbourList(i, true, true);
		}
	}
	timer.stop();
	msg.print("--> Cell atom neighbour lists generated (%s).\n", timer.timeString());
}