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
	Vec3<double> vecji, vecjk, forcei, forcek;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(dUQComm::World);
	stride = Comm.interleavedLoopStride(dUQComm::World);

	// Bonds
	Bond* b;
	for (int n=start; n<cfg.nBonds(); n += stride)
	{
		b = cfg.bond(n);

		vecji = b->j()->r() - b->i()->r();
		distance = vecji.magnitude();
		intraRDFMatrix_.ref(potentialMap_.type(b->i()->index()), potentialMap_.type(b->j()->index())).add(distance);
	}

// 	// Angles???
// 	Angle* a;
// 	for (int n=start; n<cfg.nAngles(); n += stride)
// 	{
// 		a = cfg.angle(n);

// 	for (int n=0; n<typeIndex_.nItems(); ++n)
// 	{
// 		for (int m=0; m<typeIndex_.nItems(); ++m)
// 		{
// 			Dnchar filename(-1, "%s-intra-%s-%s.rdf", "test", typeIndex_[n]->name(), typeIndex_[m]->name());
// 			intraRDFMatrix_.ref(n,m).normalisedData().save(filename);
// 			
// 			// Perform FT
// 			Data2D ft = intraRDFMatrix_.ref(n,m).normalisedData();
// 			ft.transformRDF(atomicDensity());
// 			filename.sprintf("%s-intra-%s-%s.sq", "test", typeIndex_[n]->name(), typeIndex_[m]->name());
// 			ft.save(filename);
// 		}
// 	}

	return TRUE;
}