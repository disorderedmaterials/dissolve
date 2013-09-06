/*
	*** dUQ - Partials (Intramolecular)
	*** src/main/partials_intra.cpp
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
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/species.h"
#include "base/comms.h"

/*!
 * \brief Calculate intramolecular RDFs
 */
bool DUQ::calculateIntramolecularRDFs(Configuration& cfg)
{
	// Calculate intramolecular RDFs
	double distance;
	int start, stride;
	const Box* box = cfg.box();

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Loop over molecules...
	int n;
	Atom* i, *j, *k;
	for (int m=start; m<cfg.nMolecules(); m += stride)
	{
		Molecule* mol = cfg.molecule(m);

		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			i = mol->atom(b->indexI());
			j = mol->atom(b->indexJ());
			if (configuration_.useMim(i->grain()->cell(), j->grain()->cell())) distance = box->minimumDistance(i, j);
			else distance = (i->r() - j->r()).magnitude();
			boundRDFMatrix_.ref(i->atomTypeIndex(), j->atomTypeIndex()).add(distance);
		}

		// Angles
		for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
		{
			i = mol->atom(a->indexI());
			j = mol->atom(a->indexJ());
			k = mol->atom(a->indexK());
			
			// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
			if (configuration_.useMim(i->grain()->cell(), k->grain()->cell())) distance = box->minimumDistance(i, k);
			else distance = (i->r() - k->r()).magnitude();
			boundRDFMatrix_.ref(i->atomTypeIndex(), k->atomTypeIndex()).add(distance);
		}
	}

	return true;
}