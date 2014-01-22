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
	// Grab some values from the source configuration
	const Box* box = cfg.box();
	Matrix3 rAxes = box->reciprocalAxes();
	int nAtoms = cfg.nAtoms();
	Atom* atoms = cfg.atoms();
	Vec3<double> rLengths = box->reciprocalAxisLengths();
	int n, h, k, l;

	// Set start/skip for parallel loop
	int start, stride;
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	cfg.box()->reciprocalAxes().print();

	// Calculate number of k-vectors within cutoff range
	Vec3<double> kVec;
	double mag;
	if (braggKVectors_.nItems() == 0)
	{
		msg.print("--> Performing initial setup of Bragg calculation...");

		// Determine extents of hkl indices to use
		
		braggMaximumHKL_.x = braggMaximumQ_ / rLengths.x;
		braggMaximumHKL_.y = braggMaximumQ_ / rLengths.y;
		braggMaximumHKL_.z = braggMaximumQ_ / rLengths.z;
		msg.print("Max HKL = %i %i %i\n", braggMaximumHKL_.x, braggMaximumHKL_.y, braggMaximumHKL_.z);

		braggKVectors_.clear();
		for (h = 0; h <= braggMaximumHKL_.x; ++h)
		{
			kVec.x = h * rLengths.x;
			for (k = -braggMaximumHKL_.y; k <= braggMaximumHKL_.y; ++k)
			{
				kVec.y = k * rLengths.y;
				for (l = -braggMaximumHKL_.z; l <= braggMaximumHKL_.z; ++l)
				{
					kVec.z = l * rLengths.z;
					
					// Calculate magnitude of this k vector
					mag = kVec.magnitude();
					if (mag <= braggMaximumQ_) braggKVectors_.add()->set(h, k, l, mag);
				}
			}
		}
		msg.print("--> Bragg calculation spans %i k-vectors within cutoff of Q = %f.\n", braggKVectors_.nItems(), braggMaximumQ_);

		// Create atom working arrays
		braggAtomVectorXCos_.initialise(braggMaximumHKL_.x+1, nAtoms);
		braggAtomVectorYCos_.initialise(braggMaximumHKL_.y+1, nAtoms);
		braggAtomVectorZCos_.initialise(braggMaximumHKL_.z+1, nAtoms);
		braggAtomVectorXSin_.initialise(2*braggMaximumHKL_.x+1, nAtoms);
		braggAtomVectorYSin_.initialise(2*braggMaximumHKL_.y+1, nAtoms);
		braggAtomVectorZSin_.initialise(2*braggMaximumHKL_.z+1, nAtoms);
	}

	// Create atom vectors
	Vec3<double> r;
	for (int n=0; n<nAtoms; ++n)
	{
		// Calculate (local) reciprocal atom position
// 		r.x = rAxes.columnAsVec3(0) * atoms[n].r().x;
// 		r.y = rAxes.columnAsVec3(1) * atoms[n].r().y;
// 		r.z = rAxes.columnAsVec3(2) * atoms[n].r().z;

		// Set central k vector values for the atom to cmplx(1.0,0.0)
		braggAtomVectorXCos_.ref(0, n) = 1.0;
		braggAtomVectorYCos_.ref(0, n) = 1.0;
		braggAtomVectorZCos_.ref(0, n) = 1.0;
		braggAtomVectorXSin_.ref(braggMaximumHKL_.x, n) = 0.0;
		braggAtomVectorYSin_.ref(braggMaximumHKL_.y, n) = 0.0;
		braggAtomVectorZSin_.ref(braggMaximumHKL_.z, n) = 0.0;
	}

	return true;
}