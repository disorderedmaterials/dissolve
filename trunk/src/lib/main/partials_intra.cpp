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
#include "classes/box.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/atomtype.h"
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

	// Bonds
	Bond* b;
	for (int n=start; n<cfg.nBonds(); n += stride)
	{
		b = cfg.bond(n);

		if (box->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) distance = box->minimumDistance(b->i(), b->j());
		else distance = (b->i()->r() - b->j()->r()).magnitude();
		boundRDFMatrix_.ref(potentialMap_.type(b->i()->index()), potentialMap_.type(b->j()->index())).add(distance);
	}

	// Angles
	Angle* a;
	for (int n=start; n<cfg.nAngles(); n += stride)
	{
		a = cfg.angle(n);

		// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
		if (box->useMim(a->i()->grain()->cell(), a->k()->grain()->cell())) distance = box->minimumDistance(a->i(), a->k());
		else distance = (a->i()->r() - a->k()->r()).magnitude();
	
		boundRDFMatrix_.ref(potentialMap_.type(a->i()->index()), potentialMap_.type(a->k()->index())).add(distance);
	}

	return TRUE;
}