/*
	*** BraggSQ Module - Functions
	*** src/modules/braggsq/functions.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/braggsq/braggsq.h"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggpeak.h"
#include "classes/cell.h"
#include "classes/kvector.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "math/broadeningfunction.h"
#include "math/filters.h"
#include "templates/array3d.h"
#include "templates/genericlisthelper.h"

/*
 * Private Functions
 */

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggSQModule::calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax, Vec3<int> multiplicity, bool& alreadyUpToDate)
{
	// Check to see if the arrays are up-to-date
	int& braggDataVersion = GenericListHelper<int>::retrieve(cfg->moduleData(), "BraggVersion", "", -1);
	alreadyUpToDate = braggDataVersion == cfg->contentsVersion();
	if (alreadyUpToDate) return true;

	// Realise the arrays from the Configuration
	Array<KVector>& braggKVectors = GenericListHelper< Array<KVector> >::realise(cfg->moduleData(), "BraggKVectors");
	Array<BraggPeak>& braggPeaks = GenericListHelper< Array<BraggPeak> >::realise(cfg->moduleData(), "BraggPeaks");
	Array2D<double>& braggAtomVectorXCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXCos");
	Array2D<double>& braggAtomVectorYCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYCos");
	Array2D<double>& braggAtomVectorZCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZCos");
	Array2D<double>& braggAtomVectorXSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXSin");
	Array2D<double>& braggAtomVectorYSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYSin");
	Array2D<double>& braggAtomVectorZSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZSin");
	Vec3<int>& braggMaximumHKL = GenericListHelper< Vec3<int> >::realise(cfg->moduleData(), "BraggMaximumHKL");

	// Grab some useful values
	const Box* box = cfg->box();
	int nTypes = cfg->nUsedAtomTypes();
	int nAtoms = cfg->nAtoms();
	Atom** atoms = cfg->atoms().array();

	// Set up reciprocal axes and lengths - take those from the Box and scale based on the multiplicity
	Matrix3 rAxes = box->reciprocalAxes();
	rAxes.columnMultiply(0, multiplicity.x);
	rAxes.columnMultiply(1, multiplicity.y);
	rAxes.columnMultiply(2, multiplicity.z);
	Vec3<double> rLengths = box->reciprocalAxisLengths();
	rLengths.x *= multiplicity.x;
	rLengths.y *= multiplicity.y;
	rLengths.z *= multiplicity.z;
	Messenger::print("BraggSQ: Reciprocal axes and lengths (accounting for multiplicity) are:\n");
	Messenger::print("BraggSQ:        r(x) = %e %e %e (%e)\n", rAxes.columnAsVec3(0).x, rAxes.columnAsVec3(0).y, rAxes.columnAsVec3(0).z, rLengths.x);
	Messenger::print("BraggSQ:        r(y) = %e %e %e (%e)\n", rAxes.columnAsVec3(1).x, rAxes.columnAsVec3(1).y, rAxes.columnAsVec3(1).z, rLengths.y);
	Messenger::print("BraggSQ:        r(z) = %e %e %e (%e)\n", rAxes.columnAsVec3(2).x, rAxes.columnAsVec3(2).y, rAxes.columnAsVec3(2).z, rLengths.z);

	int n, m, h, k, l, kAbs, lAbs;
	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL;
	double hkCos, hkSin, hklCos, hklSin;

	// Create a timer
	Timer timer;

	// Calculate k-vectors within specified Q range
	double mag, magSq;
	int braggIndex;
	timer.start();
	if (braggKVectors.nItems() == 0)
	{
		Messenger::print("BraggSQ: Performing initial set up of Bragg arrays...\n");
		timer.start();

		double qMaxSq = qMax*qMax, qMinSQ = qMin*qMin;
		int nBraggBins = qMax / qDelta + 1;

		// Determine extents of hkl indices to use
		braggMaximumHKL.x = qMax / rLengths.x;
		braggMaximumHKL.y = qMax / rLengths.y;
		braggMaximumHKL.z = qMax / rLengths.z;

		// Clear old arrays, and set a suitable chunk size for the k-vectors array
		braggPeaks.clear();
		braggKVectors.clear();
		braggKVectors.setChunkSize(braggMaximumHKL.x * braggMaximumHKL.y * braggMaximumHKL.z);

		// Create temporary 3D array for k-vectors, and linear array for Bragg peaks
		OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z, braggMaximumHKL.z);
		Array<BraggPeak> tempPeaks(nBraggBins);

		// Initialise Bragg peaks - Q values reflect the centre-bins of the peak.
		double q = 0.5 * qDelta;
		for (int n=0; n<nBraggBins; ++n)
		{
			tempPeaks[n].initialise(q, -1, nTypes);
			q += qDelta;
		}
		Vec3<double> kVec, v;
		for (h = 0; h <= braggMaximumHKL.x; ++h)
		{
			kVec.x = h;
			for (k = -braggMaximumHKL.y; k <= braggMaximumHKL.y; ++k)
			{
				kVec.y = k;
				for (l = -braggMaximumHKL.z; l <= braggMaximumHKL.z; ++l)
				{
					if ((h == 0) && (k == 0) && (l == 0)) continue;

					kVec.z = l;
					v = rAxes * kVec;

					// Calculate magnitude of this k vector
					magSq = v.magnitudeSq();
					if ((magSq >= qMinSQ) && (magSq <= qMaxSq))
					{
						mag = sqrt(magSq);

						// Calculate index of associated Bragg peak in the peaks array
						braggIndex = int(mag/qDelta);

						// Point this (h,k,l) value to this Bragg peak
						tempKVectors.at(h, k, l).initialise(h, k, l, braggIndex, nTypes);

						// Note in the peak that we have found another (h,k,l) that contributes to it
						tempPeaks[braggIndex].addKVectors(1);
					}
				}
			}
		}

		// Renumber peaks in BraggPeak array, assigning an index only if there are KVectors associated with it
		braggIndex = 0;
		for (int n=0; n<nBraggBins; ++n) if (tempPeaks[n].nKVectors() > 0) tempPeaks[n].setIndex(braggIndex++);

		// Collapse KVectors into a linear list, excluding any that weren't initialised
		KVector* kVectorsLinear = tempKVectors.linearArray();
		for (int n=0; n< tempKVectors.linearArraySize(); ++n)
		{
			if (kVectorsLinear[n].braggPeakIndex() == -1) continue;

			// Look up and set the new index of the associated BraggPeak
			kVectorsLinear[n].setBraggPeakIndex(tempPeaks[kVectorsLinear[n].braggPeakIndex()].index());
			braggKVectors.add(kVectorsLinear[n]);
		}

		// Prune BraggPeaks array, putting them into a sequential Array that will reflect their new indexing
		for (int n=0; n<nBraggBins; ++n)
		{
			if (tempPeaks[n].nKVectors() == 0) continue;
			braggPeaks.add(tempPeaks[n]);
		}

		Messenger::print("BraggSQ: Bragg calculation spans %i k-vectors (max HKL = %i x %i x %i) over %f <= Q <= %f (%s elapsed).\n", braggKVectors.nItems(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, qMin, qMax, timer.elapsedTimeString());
		Messenger::print("BraggSQ: %i unique Bragg peaks found using a Q resolution of %f Angstroms**-1)\n", braggPeaks.nItems(), qDelta);

		// Create atom working arrays
		braggAtomVectorXCos.initialise(nAtoms, braggMaximumHKL.x+1);
		braggAtomVectorYCos.initialise(nAtoms, braggMaximumHKL.y+1);
		braggAtomVectorZCos.initialise(nAtoms, braggMaximumHKL.z+1);
		braggAtomVectorXSin.initialise(nAtoms, 2* braggMaximumHKL.x+1);
		braggAtomVectorYSin.initialise(nAtoms, 2* braggMaximumHKL.y+1);
		braggAtomVectorZSin.initialise(nAtoms, 2* braggMaximumHKL.z+1);
	}

	// Precalculate cos/sin terms for atoms
	timer.stop();
	timer.zero();
	timer.start();
	Vec3<double> v, rI;
	for (n = 0; n<nAtoms; ++n)
	{
		// Calculate reciprocal lattice atom coordinates
		// TODO CHECK Test this in a non-cubic system!
		v = atoms[n]->r();
		rI.x = v.x*rAxes[0] + v.y*rAxes[1] + v.z*rAxes[2];
		rI.y = v.x*rAxes[3] + v.y*rAxes[4] + v.z*rAxes[5];
		rI.z = v.x*rAxes[6] + v.y*rAxes[7] + v.z*rAxes[8];

		// Grab pointers to cos/sin arrays for atom
		cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
		cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
		cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
		sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
		sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
		sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

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
		for (m=2; m<= braggMaximumHKL.x; ++m)
		{
			cosTermsH[m] = cosTermsH[1] * cosTermsH[m-1] - sinTermsH[1] * sinTermsH[m-1];
			sinTermsH[m] = cosTermsH[1] * sinTermsH[m-1] + sinTermsH[1] * cosTermsH[m-1];
			sinTermsH[-m] = -sinTermsH[m];
		}
		// Generate K terms via power expansion
		for (m=2; m<= braggMaximumHKL.y; ++m)
		{
			cosTermsK[m] = cosTermsK[1] * cosTermsK[m-1] - sinTermsK[1] * sinTermsK[m-1];
			sinTermsK[m] = cosTermsK[1] * sinTermsK[m-1] + sinTermsK[1] * cosTermsK[m-1];
			sinTermsK[-m] = -sinTermsK[m];
		}
		// Generate L terms via power expansion
		for (m=2; m<= braggMaximumHKL.z; ++m)
		{
			cosTermsL[m] = cosTermsL[1] * cosTermsL[m-1] - sinTermsL[1] * sinTermsL[m-1];
			sinTermsL[m] = cosTermsL[1] * sinTermsL[m-1] + sinTermsL[1] * cosTermsL[m-1];
			sinTermsL[-m] = -sinTermsL[m];
		}
	}
	timer.stop();
	Messenger::print("BraggSQ: Calculated atomic cos/sin terms (%s elapsed)\n", timer.totalTimeString());

	// Calculate k-vector contributions
	KVector* kVectors = braggKVectors.array();
	const int nKVectors = braggKVectors.nItems();
	int localTypeIndex;

	// Zero kvector cos/sin contributions
	for (m = 0; m < nKVectors; ++m) kVectors[m].zeroCosSinTerms();

	// Loop over atoms
	timer.start();
	for (n = 0; n<nAtoms; ++n)
	{
		// Grab localTypeIndex and array pointers for this atom
		localTypeIndex = atoms[n]->localTypeIndex();
		cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
		cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
		cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
		sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
		sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
		sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

		// Loop over k-vectors
		for (m = 0; m < nKVectors; ++m)
		{
			KVector& kvec = kVectors[m];

			// Grab h, k, and l indices from KVector
			h = kvec.h();
			k = kvec.k();
			l = kvec.l();
			kAbs = abs(k);
			lAbs = abs(l);

			// Calculate complex product from atomic cos/sin terms
			hkCos = cosTermsH[h] * cosTermsK[kAbs] - sinTermsH[h] * sinTermsK[k];
			hkSin = cosTermsH[h] * sinTermsK[k] + sinTermsH[h] * cosTermsK[kAbs];
			hklCos = hkCos * cosTermsL[lAbs] - hkSin * sinTermsL[l];
			hklSin = hkCos * sinTermsL[l] + hkSin * cosTermsL[lAbs];

			// Sum contribution into the k-vector's cos/sin arrays
			kvec.addCosTerm(localTypeIndex, hklCos);
			kvec.addSinTerm(localTypeIndex, hklSin);
		}
	}
	timer.stop();
	Messenger::print("BraggSQ: Calculated atomic contributions to k-vectors (%s elapsed)\n", timer.totalTimeString());

	// Zero Bragg peak intensities
	BraggPeak* peaks = braggPeaks.array();
	int nPeaks = braggPeaks.nItems();
	for (m=0; m<nPeaks; ++m) peaks[m].resetIntensities();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
	for (m = 0; m < nKVectors; ++m) kVectors[m].calculateIntensities(peaks);

	// Normalise intensities against number of atoms, atom type fraction, and multiplicity
	AtomTypeData* atd1 = cfg->usedAtomTypes();
	for (int i = 0; i<cfg->nUsedAtomTypes(); ++i, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (int j = i; j<cfg->nUsedAtomTypes(); ++j, atd2 = atd2->next)
		{
			double factor = (atd1->fraction() * atd2->fraction()) / (nAtoms * multiplicity.x * multiplicity.y * multiplicity.z);
			if (i != j) factor *= 2.0;

			for (m=0; m<nPeaks; ++m) peaks[m].scaleIntensity(i, j, factor);
		}
	}

	// Store the new version of the data
	braggDataVersion = cfg->contentsVersion();

	return true;
}

// // Calculate unweighted Bragg partials from pre-stored BraggPeak data
// bool BraggSQModule::calculateUnweightedBraggSQ(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax, const BroadeningFunction& broadening)
// {
// 	// Retrieve BraggPeak data from the Configuration's module data
// 	bool found = false;
// 	const Array<BraggPeak>& braggPeaks = GenericListHelper< Array<BraggPeak> >::value(cfg->moduleData(), "BraggPeaks", "", Array<BraggPeak>(), &found);
// 	if (!found) return Messenger::error("Failed to find BraggPeak array in module data for Configuration '%s'.\n", cfg->name());
// 
// 	const int nPeaks = braggPeaks.nItems();
// 	const int nTypes = cfg->nUsedAtomTypes();
// 
// 	// Realise / retrieve storage array for the Bragg partial S(Q)
// 	bool wasCreated;
// 	Array2D< Data1D >& braggSQ = GenericListHelper< Array2D< Data1D > >::realise(cfg->moduleData(), "BraggSQ", "", GenericItem::InRestartFileFlag, &wasCreated);
// 	if (wasCreated)
// 	{
// 		// Create the triangular array
// 		braggSQ.initialise(nTypes, nTypes, true);
// 	
// 		// Generate empty Data1D over the Q range / resolution specified
// 		Data1D temp;
// 		double q = qMin;
// 		while (q <= qMax)
// 		{
// 			temp.addPoint(q, 0.0);
// 			q += qDelta;
// 		}
// 
// 		// Set up Data1D array with our empty data
// 		for (int n=0; n<braggSQ.linearArraySize(); ++n) braggSQ.linearArray()[n] = temp;
// 	}
// 
// // 	AtomTypeList atomTypes = partialsq.atomTypes();
// 
// 	// Loop over pairs of atom types, constructing Bragg partial SQ for each
// 	double factor, qCentre, inten;
// 	AtomTypeData* atd1 = cfg->usedAtomTypes();
// 	for (int typeI = 0; typeI < nTypes; ++typeI, atd1 = atd1->next)
// 	{
// 		AtomTypeData* atd2 = atd1;
// 		for (int typeJ = typeI; typeJ < nTypes; ++typeJ, atd2 = atd2->next)
// 		{
// 			Data1D& partial = braggSQ.at(typeI, typeJ);
// 
// 			// Loop over defined Bragg peaks
// 			for (int n=0; n<nPeaks; ++n)
// 			{
// 				// Get q value and intensity of peak
// 				qCentre = braggPeaks.constAt(n).q();
// 				inten = braggPeaks.constAt(n).intensity(typeI, typeJ);
// 
// // 				Filters::convolveNormalised();
// 				// Loop over Q values
// 				for (int m=0; m<partial.nValues(); ++m)
// 				{
// 					// Get q value from array
// // 					q = braggSQ.x(m);
// 
// 					// No broadening
// // 					if (broadeningType == BraggSQModule::NoBroadening)
// // 					{
// // 						if (int(qCentre/qDelta) == m)
// // 						{
// // 							braggSQ.addY(m, inten);
// // 							break;
// // 						}
// // 					}
// // 					else if (broadeningType == BraggSQModule::GaussianBroadening)
// // 					{
// // 						// broadeningA = FWHM
// // 						x = qCentre - q;
// // 						braggSQ.addY(m, inten * exp(-4.0*log(2.0) * (x * x) / (fwhm * fwhm)));
// // 					}
// // 
// // 					// Set up Lorentzian parameters
// // 					qSub = (qCentre - q) / lambda;
// // 					qAdd = (qCentre + q) / lambda;
// // 					broaden = lFactor / ((1.0 + qSub*qSub) * (1.0 + qAdd*qAdd) * lambdaCubed);
// // 					printf("y = %f   %f\n", braggSQ.y(m), inten*broaden);
// // 					braggSQ.addY(m, inten * broaden);
// 				}
// 			}
// 
// 			// Normalise to atomic fractions
// 			factor = atd1->fraction() * atd2->fraction() * (typeI == typeJ ? 1.0 : 2.0);
// 			partial.values() *= factor;
// 		}
// 	}
// 
// 	return true;
// }
