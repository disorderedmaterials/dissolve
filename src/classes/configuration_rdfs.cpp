/*
	*** dUQ Methods - Structure Factor
	*** src/main/fq.cpp
	Copyright T. Youngs 2012-2016

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
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "base/timer.h"

// // Calculate Bragg peak contributions
// bool Configuration::calculateBraggContributions(ProcessPool& procPool)
// {
// 	// Grab some useful values
// 	Matrix3 rAxes = box_->reciprocalAxes();
// 	Vec3<double> rI, v, rLengths = box_->reciprocalAxisLengths();
// 	int nTypes = usedAtomTypes_.nItems();
// 
// 	int n, m, h, k, l, hAbs, kAbs, lAbs, typeI, typeJ;
// 	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL, *cosTerms, *sinTerms;
// 	double hkCos, hkSin, hklCos, hklSin;
// 	KVector* kVector;
// 
// 	// Set start/skip for parallel loop
// 	int start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
// 	int stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);
// 
// 	// Create a timer
// 	Timer timer;
// 
// 	rAxes.print();
// 
// 	// Calculate number of k-vectors within cutoff range
// 	double mag, magSq, braggMaxSq = braggMaximumQ_*braggMaximumQ_;
// 	int braggIndex;
// 	if (braggKVectors_.nItems() == 0)
// 	{
// 		Messenger::print("--> Performing initial setup of Bragg calculation...\n");
// 
// 		// Determine extents of hkl indices to use
// 		braggMaximumHKL_.x = braggMaximumQ_ / rLengths.x;
// 		braggMaximumHKL_.y = braggMaximumQ_ / rLengths.y;
// 		braggMaximumHKL_.z = braggMaximumQ_ / rLengths.z;
// 
// 		braggKVectors_.clear();
// 		Vec3<double> kVec;
// 		for (h = 0; h <= braggMaximumHKL_.x; ++h)
// 		{
// 			kVec.x = h;
// 			for (k = -braggMaximumHKL_.y; k <= braggMaximumHKL_.y; ++k)
// 			{
// 				kVec.y = k;
// 				for (l = -braggMaximumHKL_.z; l <= braggMaximumHKL_.z; ++l)
// 				{
// 					if ((h == 0) && (k == 0) && (l == 0)) continue;
// 
// 					kVec.z = l;
// 					v = rAxes * kVec;
// 
// 					// Calculate magnitude of this k vector
// 					magSq = v.magnitudeSq();
// 					if (magSq <= braggMaxSq)
// 					{
// 						mag = sqrt(magSq);
// 
// 						// Calculate integer BraggPeak index and see if a BraggPeak already exists at this position
// 						braggIndex = int(mag*10000);
// 						BraggPeak* peak = braggPeaks_.objectWithIndex(braggIndex);
// 						if (peak == NULL)
// 						{
// 							peak = braggPeaks_.add(BraggPeak(mag, braggIndex));
// 							peak->initialise(nTypes);
// 						}
// 						kVector = new KVector(h, k, l, peak, nTypes);
// 						braggKVectors_.own(kVector);
// 					}
// 				}
// 			}
// 		}
// 		Messenger::print("--> Bragg calculation spans %i hkl indices (max HKL = %i x %i x %i) within cutoff of Q = %f.\n", braggKVectors_.nItems(), braggMaximumHKL_.x, braggMaximumHKL_.y, braggMaximumHKL_.z, braggMaximumQ_);
// 
//                 // Create atom working arrays
// 		braggAtomVectorXCos_.initialise(nAtoms_, braggMaximumHKL_.x+1);
// 		braggAtomVectorYCos_.initialise(nAtoms_, braggMaximumHKL_.y+1);
// 		braggAtomVectorZCos_.initialise(nAtoms_, braggMaximumHKL_.z+1);
// 		braggAtomVectorXSin_.initialise(nAtoms_, 2*braggMaximumHKL_.x+1);
// 		braggAtomVectorYSin_.initialise(nAtoms_, 2*braggMaximumHKL_.y+1);
// 		braggAtomVectorZSin_.initialise(nAtoms_, 2*braggMaximumHKL_.z+1);
// 	}
// 
// 	// Precalculate cos/sin terms for atoms
// 	procPool.resetAccumulatedTime();
// 	timer.stop();
// 	for (n = 0; n<nAtoms_; ++n)
// 	{
// 		// Calculate reciprocal lattice atom coordinates
// 		v = atoms_[n].r();
// 		rI.x = v.x*rAxes[0] + v.y*rAxes[1] + v.z*rAxes[2];
// 		rI.y = v.x*rAxes[3] + v.y*rAxes[4] + v.z*rAxes[5];
// 		rI.z = v.x*rAxes[6] + v.y*rAxes[7] + v.z*rAxes[8];
// 
// 		// Grab pointers to cos/sin arrays for atom
// 		cosTermsH = braggAtomVectorXCos_.ptr(n, 0);
// 		cosTermsK = braggAtomVectorYCos_.ptr(n, 0);
// 		cosTermsL = braggAtomVectorZCos_.ptr(n, 0);
// 		sinTermsH = braggAtomVectorXSin_.ptr(n, braggMaximumHKL_.x);
// 		sinTermsK = braggAtomVectorYSin_.ptr(n, braggMaximumHKL_.y);
// 		sinTermsL = braggAtomVectorZSin_.ptr(n, braggMaximumHKL_.z);
// 
// 		// Initialise zero and first terms
// 		cosTermsH[0] = 1.0;
// 		cosTermsK[0] = 1.0;
// 		cosTermsL[0] = 1.0;
// 		sinTermsH[0] = 0.0;
// 		sinTermsK[0] = 0.0;
// 		sinTermsL[0] = 0.0;
// 		cosTermsH[1] = cos(rI.x);
// 		cosTermsK[1] = cos(rI.y);
// 		cosTermsL[1] = cos(rI.z);
// 		sinTermsH[1] = sin(rI.x);
// 		sinTermsK[1] = sin(rI.y);
// 		sinTermsL[1] = sin(rI.z);
// 		sinTermsH[-1] = -sinTermsH[1];
// 		sinTermsK[-1] = -sinTermsK[1];
// 		sinTermsL[-1] = -sinTermsL[1];
// 
// 		// Generate H terms via power expansion
// 		for (m=2; m<=braggMaximumHKL_.x; ++m)
// 		{
// 			cosTermsH[m] = cosTermsH[1] * cosTermsH[m-1] - sinTermsH[1] * sinTermsH[m-1];
// 			sinTermsH[m] = cosTermsH[1] * sinTermsH[m-1] + sinTermsH[1] * cosTermsH[m-1];
// 			sinTermsH[-m] = -sinTermsH[m];
// 		}
// 		// Generate K terms via power expansion
// 		for (m=2; m<=braggMaximumHKL_.y; ++m)
// 		{
// 			cosTermsK[m] = cosTermsK[1] * cosTermsK[m-1] - sinTermsK[1] * sinTermsK[m-1];
// 			sinTermsK[m] = cosTermsK[1] * sinTermsK[m-1] + sinTermsK[1] * cosTermsK[m-1];
// 			sinTermsK[-m] = -sinTermsK[m];
// 		}
// 		// Generate L terms via power expansion
// 		for (m=2; m<=braggMaximumHKL_.z; ++m)
// 		{
// 			cosTermsL[m] = cosTermsL[1] * cosTermsL[m-1] - sinTermsL[1] * sinTermsL[m-1];
// 			sinTermsL[m] = cosTermsL[1] * sinTermsL[m-1] + sinTermsL[1] * cosTermsL[m-1];
// 			sinTermsL[-m] = -sinTermsL[m];
// 		}
// 	}
// 	timer.stop();
// 	Messenger::print("--> Calculated atomic cos/sin terms (%s elapsed, %s comms)\n", timer.timeString(), procPool.accumulatedTimeString());
// 
// 	// Calculate k-vector contributions
// 	KVector** kVectors = braggKVectors_.array();
// 	const int nKVectors = braggKVectors_.nItems();
// 	int localTypeIndex;
// 
// 	// Zero kvector cos/sin contributions
// 	for (m = 0; m < nKVectors; ++m) kVectors[m]->zeroCosSinTerms();
// 
// 	// Loop over atoms
// 	procPool.resetAccumulatedTime();
// 	timer.start();
// 	for (n = 0; n<nAtoms_; ++n)
// 	{
// 		// Grab localTypeIndex and array pointers for this atom
// 		localTypeIndex = atoms_[n].localTypeIndex();
// 		cosTermsH = braggAtomVectorXCos_.ptr(n, 0);
// 		cosTermsK = braggAtomVectorYCos_.ptr(n, 0);
// 		cosTermsL = braggAtomVectorZCos_.ptr(n, 0);
// 		sinTermsH = braggAtomVectorXSin_.ptr(n, braggMaximumHKL_.x);
// 		sinTermsK = braggAtomVectorYSin_.ptr(n, braggMaximumHKL_.y);
// 		sinTermsL = braggAtomVectorZSin_.ptr(n, braggMaximumHKL_.z);
// 
// 		// Loop over k-vectors
// 		for (m = 0; m < nKVectors; ++m)
// 		{
// 			kVector = kVectors[m];
// 
// 			// Grab h, k, and l indices from KVector
// 			h = kVector->h();
// 			k = kVector->k();
// 			l = kVector->l();
// 			kAbs = abs(k);
// 			lAbs = abs(l);
// 
// 			// Calculate complex product from atomic cos/sin terms
// 			hkCos = cosTermsH[h] * cosTermsK[kAbs] - sinTermsH[h] * sinTermsK[k];
// 			hkSin = cosTermsH[h] * sinTermsK[k] + sinTermsH[h] * cosTermsK[kAbs];
// 			hklCos = hkCos * cosTermsL[lAbs] - hkSin * sinTermsL[l];
// 			hklSin = hkCos * sinTermsL[l] + hkSin * cosTermsL[lAbs];
// 
// 			// Sum contribution into the k-vector's cos/sin arrays
// 			kVector->addCosTerm(localTypeIndex, hklCos);
// 			kVector->addSinTerm(localTypeIndex, hklSin);
// 		}
// 	}
// 	timer.stop();
// 	Messenger::print("--> Calculated atomic contributions to k-vectors (%s elapsed, %s comms)\n", timer.timeString(), procPool.accumulatedTimeString());
// 
// 	// Zero Bragg peak intensities
// 	BraggPeak** peaks = braggPeaks_.objects();
// 	int nPeaks = braggPeaks_.nItems();
// 	for (m=0; m<nPeaks; ++m) peaks[m]->resetIntensities();
// 
// 	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
// 	for (m = 0; m < nKVectors; ++m) kVectors[m]->calculateIntensities();
// 
// 	return true;
// }
// 
// // Calculate Bragg S(Q) into supplied matrix, using specified broadening
// bool Configuration::calculateBraggSQ(const AtomTypeList& sampleAtomTypes, Array2D<Data2D>& braggMatrix, double broadening)
// {
// 	double factor, qCentre, q, inten, qSub, qAdd, broaden, lFactor, lambda, lambdaCubed;
// 	int typeI, typeJ, nTypes;

// 	XXX Since each Configuration (and Sample) maintains its own AtomTypeIndex (which may contain only a subset of the AtomTypes in the Master AtomTypeIndex)
// 	XXX we need to check the types used in the Sample against those in the Configuration. If there are less AtomTypes in the Configuration than in the Sample we don't mind
// 	XXX (it might be that we are mixing Configurations in order to get the Sample F(Q)) but if there are more we must object since they will have no Isotopologue information
// 	XXX in the Sample, and we will not know how to weight them.

// 	lFactor = box_->reciprocalVolume() / (PI*PI);
// 
// 	printf("TODO Use of instrumental Q-dependent broadening needs to be re-implemented per Sample in Bragg calculation\n");
// 	for (typeI = 0; typeI < nTypes; ++typeI)
// 	{
// 		for (typeJ = typeI; typeJ < nTypes; ++typeJ)
// 		{
// 			Data2D& braggSQ = braggSQMatrix_.ref(typeI, typeJ);
// 
// 			// Calculate prefactor
// 			factor = (typeI == typeJ ? 1.0 : 2.0);
// 
// 			// Loop over defined Bragg peaks
// 			for (n=0; n<nPeaks; ++n)
// 			{
// 				// Get q value and intensity of peak
// 				qCentre = peaks[n]->q();
// 				inten = peaks[n]->intensity(typeI, typeJ);
// 				lambda = braggBroadening_ + qCentre * 0.02; //qDependentFWHM_;
// 				lambdaCubed = lambda * lambda * lambda;
// 
// 				// Loop over points in braggSQ Data2D (which will provide our x-values)
// 				for (m=0; m<braggSQ.nPoints(); ++m)
// 				{
// 					// Get q value from array
// 					q = braggSQ.x(m);
// 
// 					// Set up Lorentzian parameters
// 					qSub = (qCentre - q) / lambda;
// 					qAdd = (qCentre + q) / lambda;
// 					broaden = lFactor / ((1.0 + qSub*qSub) * (1.0 + qAdd*qAdd) * lambdaCubed);
// 					braggSQ.addY(m, inten * broaden);
// 				}
// 			}
// 
// 			// Normalise to total number of atoms, subtract single atom scattering, and normalise to atomic fractions
// 			braggSQ.arrayY() /= nAtoms_;
// 			if (typeI == typeJ) braggSQ.arrayY() -= usedAtomTypes_[typeI]->fraction();
// 			braggSQ.arrayY() /= usedAtomTypes_[typeI]->fraction() * usedAtomTypes_[typeJ]->fraction();
// 		}
// 	}

// 	for (m=0; m<nPeaks; ++m) Messenger::print("  %f   %f\n", peaks[m]->q(), peaks[m]->intensity(0,0));

// 	return true;
// }

// Set RDF bin width
void Configuration::setRDFBinWidth(double width)
{
	rdfBinWidth_ = width;
}

// Return RDF bin width
double Configuration::rdfBinWidth()
{
	return rdfBinWidth_;
}

// Return working RDF extent
double Configuration::rdfRange()
{
	return rdfRange_;
}

// Set degree of smoothing to apply to calculated RDF data
void Configuration::setRDFSmoothing(int smoothingDegree)
{
	rdfSmoothing_ = smoothingDegree;
}

// Return degree of smoothing to apply to calculated RDF data
int Configuration::rdfSmoothing()
{
	return rdfSmoothing_;
}

// Set requested RDF extent
void Configuration::setRequestedRDFRange(double range)
{
	requestedRDFRange_ = range;
}

// Return requested RDF extent
double Configuration::requestedRDFRange()
{
	return requestedRDFRange_;
}

// Set maximum Q value for Bragg calculation
void Configuration::setBraggMaximumQ(double qMax)
{
	braggMaximumQ_ = qMax;
}

// Return maximum Q value for Bragg calculation
double Configuration::braggMaximumQ()
{
	return braggMaximumQ_;
}

// Return maximal extent of hkl for Bragg calculation
Vec3<int> Configuration::braggMaximumHKL()
{
	return braggMaximumHKL_;
}
