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
	Vec3<double> r, rLengths = box->reciprocalAxisLengths();
	int n, m, h, k, l, hAbs, kAbs, lAbs;
	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL; 
	double xyCos, xySin, xyzCos, xyzSin;

	// Set start/skip for parallel loop
	int start, stride;
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Create a timer
	Timer timer;

	rAxes[1] = -rAxes[1];
	cfg.box()->reciprocalAxes().print();

	// Calculate number of k-vectors within cutoff range
	double mag;
	if (braggKVectors_.nItems() == 0)
	{
		msg.print("--> Performing initial setup of Bragg calculation...\n");
		
		int nTypes = typeIndex_.nItems();

		// Determine extents of hkl indices to use
		braggMaximumHKL_.x = braggMaximumQ_ / rLengths.x;
		braggMaximumHKL_.y = braggMaximumQ_ / rLengths.y;
		braggMaximumHKL_.z = braggMaximumQ_ / rLengths.z;

		braggKVectors_.clear();
		Vec3<double> kVec;
		for (h = 0; h <= braggMaximumHKL_.x; ++h)
		{
			kVec.x = h;// * rLengths.x;
			for (k = -braggMaximumHKL_.y; k <= braggMaximumHKL_.y; ++k)
			{
				kVec.y = k;// * rLengths.y;
				for (l = -braggMaximumHKL_.z; l <= braggMaximumHKL_.z; ++l)
				{
// 					if ((h == 0) && (k == 0) && (l == 0)) continue;

					kVec.z = l;// * rLengths.z;
					r = rAxes * kVec;

					// Calculate magnitude of this k vector
					mag = r.magnitude();
					if (mag <= braggMaximumQ_) braggKVectors_.add(KVector(h, k, l, mag, nTypes));
				}
			}
		}
		msg.print("--> Bragg calculation spans %i hkl indices (%i x %i x %i) within cutoff of Q = %f.\n", braggKVectors_.nItems(), braggMaximumHKL_.x, braggMaximumHKL_.y, braggMaximumHKL_.z, braggMaximumQ_);
	}

	// Calculate k-vector contributions
	KVector** kVectors = braggKVectors_.objects();
	KVector* kVector;
	const int nKVectors = braggKVectors_.nItems();
	int atomTypeIndex;
	double multiplier;

	// Zero kvector cos/sin contributions
	for (m = 0; m < nKVectors; ++m) kVectors[m]->zeroCosSinTerms();

	// Loop over atoms
	Comm.resetAccumulatedTime();
	timer.stop();
	for (n = 0; n<nAtoms; ++n)
	{
		// Grab atomtypeindex for current atom
		atomTypeIndex = atoms[n].atomTypeIndex();

		// Loop over k-vectors
		for (m = 0; m < nKVectors; ++m)
		{
			kVector = kVectors[m];
			// Grab h, k, and l indices from KVector
			h = kVector->h();
			k = kVector->k();
			l = kVector->l();
			
			r = rAxes * atoms[n].r();
			double arg = r.x * kVectors[m]->h() + r.y * kVectors[m]->k() + r.z * kVectors[m]->l();

			// Sum contribution into the k-vector's cos/sin arrays
			kVector->addCosTerm(atomTypeIndex, cos(arg));
			kVector->addSinTerm(atomTypeIndex, -sin(arg));
		}
	}
	msg.print("--> Calculated atomic contributions to k-vectors (%s elapsed, %s comms)\n", timer.timeString(), Comm.accumulatedTimeString());

	for (m = 0; m < nKVectors; ++m)
	{
		double rAmpTot = kVectors[m]->cosTerms().sum();
		double iAmpTot = kVectors[m]->sinTerms().sum();
		msg.print(" %2i  %2i  %2i    %f   %f   %f   %f   %f\n", kVectors[m]->h(), kVectors[m]->k(), kVectors[m]->l(), kVectors[m]->magnitude(), kVectors[m]->cosTerms().value(0)/nAtoms, kVectors[m]->sinTerms().value(0)/nAtoms, kVectors[m]->cosTerms().value(1)/nAtoms, kVectors[m]->sinTerms().value(1)/nAtoms);
	}

	return true;
}