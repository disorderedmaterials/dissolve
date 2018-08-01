/*
	*** Bragg Module - Functions
	*** src/modules/bragg/functions.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/bragg/bragg.h"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggpeak.h"
#include "classes/cell.h"
#include "classes/kvector.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "math/broadeningfunction.h"
#include "templates/array3d.h"
#include "templates/genericlisthelper.h"

/*
 * Private Functions
 */

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggModule::calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, double braggQMin, double braggQResolution, double braggQMax, double braggMultiplicity)
{
	// Check to see if the arrays are up-to-date
	// TODO

	// Realise the arrays from the Configuration
	Array<KVector>& braggKVectors = GenericListHelper< Array<KVector> >::realise(cfg->moduleData(), "BraggKVectors", uniqueName());
	Array<BraggPeak>& braggPeaks = GenericListHelper< Array<BraggPeak> >::realise(cfg->moduleData(), "BraggPeaks", uniqueName());
	Array2D<double>& braggAtomVectorXCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXCos", uniqueName());
	Array2D<double>& braggAtomVectorYCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYCos", uniqueName());
	Array2D<double>& braggAtomVectorZCos = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZCos", uniqueName());
	Array2D<double>& braggAtomVectorXSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXSin", uniqueName());
	Array2D<double>& braggAtomVectorYSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYSin", uniqueName());
	Array2D<double>& braggAtomVectorZSin = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZSin", uniqueName());
	Vec3<int>& braggMaximumHKL = GenericListHelper< Vec3<int> >::realise(cfg->moduleData(), "BraggMaximumHKL", uniqueName());

	// Grab some useful values
	const Box* box = cfg->box();
	Matrix3 rAxes = box->reciprocalAxes();
	Vec3<double> rI, v, rLengths = box->reciprocalAxisLengths();
	int nTypes = cfg->nUsedAtomTypes();
	int nAtoms = cfg->nAtoms();
	Atom** atoms = cfg->atoms().array();

	int n, m, h, k, l, kAbs, lAbs;
	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL;
	double hkCos, hkSin, hklCos, hklSin;

	// Create a timer
	Timer timer;

	rAxes.print();

	// Calculate number of k-vectors within cutoff range
	double mag, magSq;
	int braggIndex;
	timer.start();
	if (braggKVectors.nItems() == 0)
	{
		Messenger::print("Partials: Performing initial set up of Bragg arrays...\n");
		timer.start();

		double braggQMaxSq = braggQMax*braggQMax, braggQMinSQ = braggQMin*braggQMin;
		int nBraggBins = braggQMax / braggQResolution + 1;

		// Determine extents of hkl indices to use
		braggMaximumHKL.x = braggQMax / rLengths.x;
		braggMaximumHKL.y = braggQMax / rLengths.y;
		braggMaximumHKL.z = braggQMax / rLengths.z;

		// Clear old arrays
		braggPeaks.clear();
		braggKVectors.clear();

		// Create temporary 3D array for k-vectors, and linear array for Bragg peaks
		OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z, braggMaximumHKL.z);
		Array<BraggPeak> tempPeaks(nBraggBins);

		// Initialise Bragg peaks
		// The Q value reflects the centre-bin value of the peak.
		double q = 0.5 * braggQResolution;
		for (int n=0; n<nBraggBins; ++n)
		{
			tempPeaks[n].initialise(q, -1, nTypes);
			q += braggQResolution;
		}
		Vec3<double> kVec;
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
					if ((magSq >= braggQMinSQ) && (magSq <= braggQMaxSq))
					{
						mag = sqrt(magSq);

						// Calculate index of associated Bragg peak in the peaks array
						braggIndex = int(mag/braggQResolution);

						// Point this (h,k,l) value to this Bragg peak
						tempKVectors.ref(h, k, l).initialise(h, k, l, braggIndex, nTypes);

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

		Messenger::print("Partials: Bragg calculation spans %i hkl indices (max HKL = %i x %i x %i) over %f <= Q <= %f (%s elapsed).\n", braggKVectors.nItems(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, braggQMin, braggQMax, timer.elapsedTimeString());
		Messenger::print("Partials: %i unique Bragg peaks found using a Q resolution of %f Angstroms**-1)\n", braggPeaks.nItems(), braggQResolution);

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
	for (n = 0; n<nAtoms; ++n)
	{
		// Calculate reciprocal lattice atom coordinates
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
	Messenger::print("Partials: Calculated atomic cos/sin terms (%s elapsed)\n", timer.totalTimeString());

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
	Messenger::print("Partials: Calculated atomic contributions to k-vectors (%s elapsed)\n", timer.totalTimeString());

	// Zero Bragg peak intensities
	BraggPeak* peaks = braggPeaks.array();
	int nPeaks = braggPeaks.nItems();
	for (m=0; m<nPeaks; ++m) peaks[m].resetIntensities();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
	for (m = 0; m < nKVectors; ++m) kVectors[m].calculateIntensities(peaks);

	// Scale intensities for number of atoms and multiplicity
	double factor = 1.0 / (nAtoms * braggMultiplicity);
	for (m=0; m<nPeaks; ++m) peaks[m].scaleIntensity(factor);

	return true;
}

// Calculate unweighted Bragg partials from calculated peak data
bool BraggModule::calculateUnweightedBraggSQ(ProcessPool& procPool, Configuration* cfg, Array< BraggPeak >& braggPeaks, PartialSet& partialsq, const BroadeningFunction& broadening)
{
	double factor, qCentre, inten;
	int nTypes = partialsq.nAtomTypes();
	AtomTypeList atomTypes = partialsq.atomTypes();
	int nPeaks = braggPeaks.nItems();
	//double qDelta = partialsq.partial(0,0).x(1) - partialsq.partial(0,0).x(0);

	// Print out a bit of info first
	Messenger::print("Partials: Calculating Bragg contributions to S(Q)...\n");
	// BRAGG TODO
	Messenger::error("BROADENING OF BRAGG SCATTERING IS CURRENTLY DISABLED - FIX ME!\n");
// 	if (broadeningType == BraggModule::NoBroadening) Messenger::warn("Partials: No broadening will be applied to Bragg contributions.\n");
// 	else if (broadeningType == BraggModule::GaussianBroadening)
// 	{
// 		fwhm = GenericListHelper<double>::retrieve(cfg->moduleData(), "BraggBroadeningParameter1", uniqueName(), 0.01);
// 		factor = sqrt(4.0 * log(2.0) / PI);
// 		Messenger::print("Partials: Gaussian broadening will be applied to Bragg contributions with FWHM = %f Angstroms**-1.\n", fwhm);
// 	}

	// Loop over pairs of atom types, constructing Bragg partial SQ for each
	for (int typeI = 0; typeI < nTypes; ++typeI)
	{
		for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
		{
			XYData& braggSQ = partialsq.braggPartial(typeI, typeJ);

			// Loop over defined Bragg peaks
			for (int n=0; n<nPeaks; ++n)
			{
				// Get q value and intensity of peak
				qCentre = braggPeaks[n].q();
				inten = braggPeaks[n].intensity(typeI, typeJ);
// 				lambda = braggQIndepBroadening + qCentre * qDepBroadening;
// 				lambdaCubed = lambda * lambda * lambda;
// 				tempBraggSQ.addY(qCentre/braggQDelta, inten);
				if (inten > 0.01) printf("BRG  %i  %i  %i  %f  %f\n", typeI, typeJ, n, qCentre, inten);

				// Loop over points in braggSQ XYData (which will provide our Q values)
				for (int m=0; m<braggSQ.nPoints(); ++m)
				{
					// Get q value from array
					//q = braggSQ.x(m);

					// No broadening
// 					if (broadeningType == BraggModule::NoBroadening)
// 					{
// 						if (int(qCentre/qDelta) == m)
// 						{
// 							braggSQ.addY(m, inten);
// 							break;
// 						}
// 					}
// 					else if (broadeningType == BraggModule::GaussianBroadening)
// 					{
// 						// broadeningA = FWHM
// 						x = qCentre - q;
// 						braggSQ.addY(m, inten * exp(-4.0*log(2.0) * (x * x) / (fwhm * fwhm)));
// 					}
// 
// 					// Set up Lorentzian parameters
// 					qSub = (qCentre - q) / lambda;
// 					qAdd = (qCentre + q) / lambda;
// 					broaden = lFactor / ((1.0 + qSub*qSub) * (1.0 + qAdd*qAdd) * lambdaCubed);
// 					printf("y = %f   %f\n", braggSQ.y(m), inten*broaden);
// 					braggSQ.addY(m, inten * broaden);
				}
			}

			// Normalise to atomic fractions
			factor = atomTypes[typeI]->fraction() * atomTypes[typeJ]->fraction() * (typeI == typeJ ? 1.0 : 2.0);
			braggSQ.arrayY() *= factor;
		}
	}

	return true;
}



// 	// Generate final partial S(Q) combining pair correlations and Bragg partials
// 	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
// 	{
// 		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
// 		{
// 			// Grab references:
// 			// -- Full atom pair-pair structure factors (bound + unbound)
// 			XYData& pairSQ = pairSQMatrix_.ref(typeI,typeJ);
// 			// -- Bragg partial structure factors
// 			XYData& braggSQ = braggSQMatrix_.ref(typeI,typeJ);
// 			// -- Full partial structure factors (destination)
// 			XYData& partialSQ = partialSQMatrix_.ref(typeI,typeJ);
// 
// 			// Copy atomic pair S(Q) information
// 			partialSQ = pairSQ;
// 
// 			// Combine Bragg(Q) data if it was calculated
// 			if (braggOn)
// 			{
// 				double xRange = 0.1, x;
// 				double xMin = braggQMax - xRange - qDelta*0.5;
// 				for (int n=0; n<braggSQ.nPoints(); ++n)
// 				{
// 					x = (braggSQ.x(n) <= xMin ? 0.0 : (braggSQ.x(n) - xMin) / xRange);
// 					partialSQ.setY(n, pairSQ.y(n)*x + braggSQ.y(n)*(1.0-x));
// 					// TEST - Straight replacement of original pair data
// // 					partialSQ.setY(n, braggSQ.y(n));
// 				}
// 			}
// 		}
// 	}

