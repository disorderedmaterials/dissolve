/*
	*** dUQ - Partials (Bragg)
	*** src/main/partials_bragg.cpp
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
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/species.h"
#include "base/comms.h"

/*!
 * \brief Calculate Bragg S(Q)
 */
bool DUQ::calculateBraggSQ(Configuration& cfg)
{
	const Box* box = cfg.box();

	// Set start/skip for parallel loop
	int start, stride;
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	cfg.box()->reciprocalAxes().print();

	// Determine extents of hkl indices to use
	Vec3<double> rLengths = box->reciprocalAxisLengths();
	int h, k, l, hMax, kMax, lMax;
	hMax = braggMaximumQ_ / rLengths.x;
	kMax = braggMaximumQ_ / rLengths.y;
	lMax = braggMaximumQ_ / rLengths.z;
	msg.print("Max HKL = %i %i %i\n", hMax, kMax, lMax);

	// Calculate number of k-vectors within cutoff range
	Vec3<double> kVec;
	if (braggKVectors_.nItems() == 0)
	{
		msg.print("--> Performing initial setup of Bragg calculation...");
		braggKVectors_.clear();
		for (h = 0; h <= hMax; ++h)
		{
			kVec.x = h * rLengths.x;
			for (k = -kMax; k <= kMax; ++k)
			{
				kVec.y = k * rLengths.y;
				for (l = -lMax; l <= lMax; ++l)
				{
					kVec.z = l * rLengths.z;
					
					// Calculate magnitude of this k vector
					if (kVec.magnitude() <= braggMaximumQ_) braggKVectors_.add( Vec3<int>(h, k, l) );
				}
			}
		}
		msg.print("--> Bragg calculation spans %i k-vectors within cutoff of Q = %f.\n", braggKVectors_.nItems(), braggMaximumQ_);

		// Create 
	}


	return true;
}