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
	// Grab some useful values
	const Box* box = cfg.box();
	Matrix3 rAxes = box->reciprocalAxes();
	int nAtoms = cfg.nAtoms();
	Atom* atoms = cfg.atoms();
	Vec3<double> rI, v, rLengths = box->reciprocalAxisLengths();
	int nTypes = typeIndex_.nItems();

	int n, m, h, k, l, hAbs, kAbs, lAbs, typeI, typeJ;
	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL, *cosTerms, *sinTerms;
	double hkCos, hkSin, hklCos, hklSin;
	KVector* kVector;

	// Set start/skip for parallel loop
	int start, stride;
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Create a timer
	Timer timer;
	
	rAxes.print();

	// Calculate number of k-vectors within cutoff range
	double mag;
	int braggIndex;
	if (braggKVectors_.nItems() == 0)
	{
		msg.print("--> Performing initial setup of Bragg calculation...\n");

		// Determine extents of hkl indices to use
		braggMaximumHKL_.x = braggMaximumQ_ / rLengths.x;
		braggMaximumHKL_.y = braggMaximumQ_ / rLengths.y;
		braggMaximumHKL_.z = braggMaximumQ_ / rLengths.z;

		braggKVectors_.clear();
		Vec3<double> kVec;
		for (h = 0; h <= braggMaximumHKL_.x; ++h)
		{
			kVec.x = h;
			for (k = -braggMaximumHKL_.y; k <= braggMaximumHKL_.y; ++k)
			{
				kVec.y = k;
				for (l = -braggMaximumHKL_.z; l <= braggMaximumHKL_.z; ++l)
				{
// 					if ((h == 0) && (k == 0) && (l == 0)) continue;

					kVec.z = l;
					v = rAxes * kVec;

					// Calculate magnitude of this k vector
					mag = v.magnitude();
					if (mag <= braggMaximumQ_)
					{
						// Calculate integer BraggPeak index and see if a BraggPeak already exists at this position
						braggIndex = int(mag*10000);
						BraggPeak* peak = braggPeaks_.objectWithIndex(braggIndex);
						if (peak == NULL)
						{
							peak = braggPeaks_.add(BraggPeak(mag, braggIndex));
							peak->initialise(nTypes);
						}
						kVector = new KVector(h, k, l, peak, nTypes);
						braggKVectors_.own(kVector);
					}
				}
			}
		}
		msg.print("--> Bragg calculation spans %i hkl indices (max HKL = %i x %i x %i) within cutoff of Q = %f.\n", braggKVectors_.nItems(), braggMaximumHKL_.x, braggMaximumHKL_.y, braggMaximumHKL_.z, braggMaximumQ_);

                // Create atom working arrays
		braggAtomVectorXCos_.initialise(nAtoms, braggMaximumHKL_.x+1);
		braggAtomVectorYCos_.initialise(nAtoms, braggMaximumHKL_.y+1);
		braggAtomVectorZCos_.initialise(nAtoms, braggMaximumHKL_.z+1);
		braggAtomVectorXSin_.initialise(nAtoms, 2*braggMaximumHKL_.x+1);
		braggAtomVectorYSin_.initialise(nAtoms, 2*braggMaximumHKL_.y+1);
		braggAtomVectorZSin_.initialise(nAtoms, 2*braggMaximumHKL_.z+1);
	}

	// Precalculate cos/sin terms for atoms
	Comm.resetAccumulatedTime();
	timer.stop();
	for (n = 0; n<nAtoms; ++n)
	{
		rI = rAxes * atoms[n].r();

		cosTermsH = braggAtomVectorXCos_.ptr(n, 0);
		cosTermsK = braggAtomVectorYCos_.ptr(n, 0);
		cosTermsL = braggAtomVectorZCos_.ptr(n, 0);
		sinTermsH = braggAtomVectorXSin_.ptr(n, braggMaximumHKL_.x);
		sinTermsK = braggAtomVectorYSin_.ptr(n, braggMaximumHKL_.y);
		sinTermsL = braggAtomVectorZSin_.ptr(n, braggMaximumHKL_.z);

		// Initialise zero and first terms
		cosTermsH[0] = 1.0;
		cosTermsK[0] = 1.0;
		cosTermsL[0] = 1.0;
		sinTermsH[0] = 0.0;
		sinTermsK[0] = 0.0;
		sinTermsL[0] = 0.0;
		cosTermsH[1] = cos(rI.x);
		cosTermsK[1] = cos(rI.y);
		cosTermsL[1] = cos(rI.z);
		sinTermsH[1] = sin(rI.x);
		sinTermsK[1] = sin(rI.y);
		sinTermsL[1] = sin(rI.z);
		sinTermsH[-1] = -sinTermsH[1];
		sinTermsK[-1] = -sinTermsK[1];
		sinTermsL[-1] = -sinTermsL[1];

		// Generate H terms via power expansion
		for (m=2; m<=braggMaximumHKL_.x; ++m)
		{
			cosTermsH[m] = cosTermsH[1] * cosTermsH[m-1] - sinTermsH[1] * sinTermsH[m-1];
			sinTermsH[m] = cosTermsH[1] * sinTermsH[m-1] + sinTermsH[1] * cosTermsH[m-1];
			sinTermsH[-m] = -sinTermsH[m];
		}
		// Generate K terms via power expansion
		for (m=2; m<=braggMaximumHKL_.y; ++m)
		{
			cosTermsK[m] = cosTermsK[1] * cosTermsK[m-1] - sinTermsK[1] * sinTermsK[m-1];
			sinTermsK[m] = cosTermsK[1] * sinTermsK[m-1] + sinTermsK[1] * cosTermsK[m-1];
			sinTermsK[-m] = -sinTermsK[m];
		}
		// Generate L terms via power expansion
		for (m=2; m<=braggMaximumHKL_.z; ++m)
		{
			cosTermsL[m] = cosTermsL[1] * cosTermsL[m-1] - sinTermsL[1] * sinTermsL[m-1];
			sinTermsL[m] = cosTermsL[1] * sinTermsL[m-1] + sinTermsL[1] * cosTermsL[m-1];
			sinTermsL[-m] = -sinTermsL[m];
		}
	}
	timer.stop();
	msg.print("--> Calculated atomic cos/sin terms (%s elapsed, %s comms)\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate k-vector contributions
	KVector** kVectors = braggKVectors_.array();
	const int nKVectors = braggKVectors_.nItems();
	int atomTypeIndex;

	// Zero kvector cos/sin contributions
	for (m = 0; m < nKVectors; ++m) kVectors[m]->zeroCosSinTerms();

	// Loop over atoms
	Comm.resetAccumulatedTime();
	timer.stop();
	for (n = 0; n<nAtoms; ++n)
	{
		// Grab atomTypeIndex and array pointers for this atom
		atomTypeIndex = atoms[n].atomTypeIndex();
		cosTermsH = braggAtomVectorXCos_.ptr(n, 0);
		cosTermsK = braggAtomVectorYCos_.ptr(n, 0);
		cosTermsL = braggAtomVectorZCos_.ptr(n, 0);
		sinTermsH = braggAtomVectorXSin_.ptr(n, braggMaximumHKL_.x);
		sinTermsK = braggAtomVectorYSin_.ptr(n, braggMaximumHKL_.y);
		sinTermsL = braggAtomVectorZSin_.ptr(n, braggMaximumHKL_.z);

		// Loop over k-vectors
		for (m = 0; m < nKVectors; ++m)
		{
			kVector = kVectors[m];

			// Grab h, k, and l indices from KVector
			h = kVector->h();
			k = kVector->k();
			l = kVector->l();
			kAbs = abs(k);
			lAbs = abs(l);

			// Calculate complex product from atomic cos/sin terms
			hkCos = cosTermsH[h] * cosTermsK[kAbs] - sinTermsH[h] * sinTermsK[k];
			hkSin = cosTermsH[h] * sinTermsK[k] + sinTermsH[h] * cosTermsK[kAbs];
			hklCos = hkCos * cosTermsL[lAbs] - hkSin * sinTermsL[l];
			hklSin = hkCos * sinTermsL[l] + hkSin * cosTermsL[lAbs];

			// Sum contribution into the k-vector's cos/sin arrays
			kVector->addCosTerm(atomTypeIndex, hklCos);
			kVector->addSinTerm(atomTypeIndex, hklSin);
		}
	}
	timer.stop();
	msg.print("--> Calculated atomic contributions to k-vectors (%s elapsed, %s comms)\n", timer.timeString(), Comm.accumulatedTimeString());

	// Zero Bragg peak intensities
	BraggPeak** peaks = braggPeaks_.objects();
	int nPeaks = braggPeaks_.nItems();
	for (m=0; m<nPeaks; ++m) peaks[m]->resetIntensities();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
	for (m = 0; m < nKVectors; ++m)
	{
		kVector = kVectors[m];

// 		msg.print(" %2i  %2i  %2i    %f   ", kVectors[m]->h(), kVectors[m]->k(), kVectors[m]->l(), kVectors[m]->braggPeak()->q());
		kVector->calculateIntensities(nAtoms);
	}

	// Create individual functions in braggSQMatrix_
	double factor, q, inten;
	for (typeI = 0; typeI < nTypes; ++typeI)
	{
		for (typeJ = typeI; typeJ < nTypes; ++typeJ)
		{
			Data2D& braggSQ = braggSQMatrix_.ref(typeI, typeJ);
			Array<double>& y = braggSQ.arrayY();

			// Calculate atomic fraction and scattering multiplier
			factor = typeIndex_[typeI]->fraction() * typeIndex_[typeJ]->fraction() * (typeI == typeJ ? 1.0 : 2.0);

			// Starting Q value will be half bin-width
			q = 0.5*qDelta_;
			while (q <= braggMaximumQ_)
			{
				// Zero the intensity accumulator
				inten = 0.0;

				// Loop over Bragg peaks and sum contributions at this value of Q
				for (n=0; n<nPeaks; ++n) inten += peaks[n]->intensity(typeI, typeJ, q);

				// Add point to braggSQ
				braggSQ.addPoint(q, inten);
				q += qDelta_;
			}
		}
	}

// 	for (m=0; m<nPeaks; ++m) msg.print("  %f   %f\n", peaks[m]->q(), peaks[m]->intensity(0,0));

	return true;
}
