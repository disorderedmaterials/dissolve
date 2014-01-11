/*
	*** dUQ Methods - Upkeep
	*** src/lib/main/methods_upkeep.cpp
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
void DUQ::updateCellAtomNeighbourLists(Configuration& cfg)
{
	// Clear all existing atoms in cell neighbour lists
	for (int n=0; n<cfg.nCells(); ++n) cfg.cell(n)->clearAtomNeighbourList();

	Vec3<double> r;
	double distSq, cutoffSq = pairPotentialRange_ + sqrt(cfg.realCellSize().dp(cfg.realCellSize()))*0.5;
	msg.print("Cutoff for atom cell neighbours is %f\n", cutoffSq);
	cutoffSq *= cutoffSq;

	// Loop over cells
	for (int n=0; n<cfg.nCells(); ++n)
	{
		Cell* cell = cfg.cell(n);
		
		// Calculate centre coordinate of this cell   TODO only for cubic systems
		Vec3<double> cellCentre = cfg.realCellSize()*0.5 + Vec3<double>(cfg.realCellSize().x*cell->gridReference().x,cfg.realCellSize().y*cell->gridReference().y,cfg.realCellSize().z*cell->gridReference().z); 

		// Loop over cell neighbours, and then over atoms, checking distance with the central cell
		for (RefListItem<Cell,bool>* ri = cell->neighbours().first(); ri != NULL; ri = ri->next)
		{
			Cell* otherCell = ri->item;

			for (int m=0; m<otherCell->maxAtoms(); ++m)
			{
				Atom* i = otherCell->atom(m);
				if (i == NULL) continue;

				// Get minimum image vector w.r.t. cell centre
				r = cfg.box()->minimumVector(cellCentre, i->r());
				distSq = r.magnitudeSq();
				if (distSq > cutoffSq) continue;
				cell->addAtomToNeighbourList(i, cfg.useMim(cell, i->cell()));
			}
		}
	}
}