/*
	*** Bragg Module - Functions
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

#include "modules/bragg/bragg.h"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggreflection.h"
#include "classes/cell.h"
#include "classes/kvector.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "math/broadeningfunction.h"
#include "math/filters.h"
#include "templates/array3d.h"
#include "genericitems/listhelper.h"

/*
 * Private Functions
 */

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggModule::calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax, Vec3<int> multiplicity, bool& alreadyUpToDate)
{
	// Check to see if the arrays are up-to-date
	int braggDataVersion = GenericListHelper<int>::retrieve(cfg->moduleData(), "BraggVersion", "", -1);
	alreadyUpToDate = braggDataVersion == cfg->contentsVersion();
	if (alreadyUpToDate) return true;

	// Realise the arrays from the Configuration
	Array<KVector>& braggKVectors = GenericListHelper< Array<KVector> >::realise(cfg->moduleData(), "BraggKVectors");
	Array<BraggReflection>& braggReflections = GenericListHelper< Array<BraggReflection> >::realise(cfg->moduleData(), "BraggReflections", "", GenericItem::InRestartFileFlag);
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
	Messenger::print("Reciprocal axes and lengths (accounting for multiplicity) are:\n");
	Messenger::print("	r(x) = %e %e %e (%e)\n", rAxes.columnAsVec3(0).x, rAxes.columnAsVec3(0).y, rAxes.columnAsVec3(0).z, rLengths.x);
	Messenger::print("	r(y) = %e %e %e (%e)\n", rAxes.columnAsVec3(1).x, rAxes.columnAsVec3(1).y, rAxes.columnAsVec3(1).z, rLengths.y);
	Messenger::print("	r(z) = %e %e %e (%e)\n", rAxes.columnAsVec3(2).x, rAxes.columnAsVec3(2).y, rAxes.columnAsVec3(2).z, rLengths.z);

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
		Messenger::print("Performing initial set up of Bragg arrays...\n");
		timer.start();

		double qMaxSq = qMax*qMax, qMinSQ = qMin*qMin;
		int nBraggBins = qMax / qDelta + 1;

		// Determine extents of hkl indices to use
		braggMaximumHKL.x = qMax / rLengths.x;
		braggMaximumHKL.y = qMax / rLengths.y;
		braggMaximumHKL.z = qMax / rLengths.z;

		// Clear old arrays, and set a suitable chunk size for the k-vectors array
		braggReflections.clear();
		braggKVectors.clear();
		braggKVectors.setChunkSize(braggMaximumHKL.x * braggMaximumHKL.y * braggMaximumHKL.z);

		// Create temporary 3D array for k-vectors, and linear array for Bragg reflections
		OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z, braggMaximumHKL.z);
		Array<BraggReflection> tempReflections(nBraggBins);

		// Initialise Bragg reflections - Q values reflect the centre-bins of the reflection.
		double q = 0.5 * qDelta;
		for (int n=0; n<nBraggBins; ++n)
		{
			tempReflections[n].initialise(q, -1, nTypes);
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

						// Calculate index of associated Bragg reflection in the reflections array
						braggIndex = int(mag/qDelta);

						// Point this (h,k,l) value to this Bragg reflection
						tempKVectors.at(h, k, l).initialise(h, k, l, braggIndex, nTypes);

						// Note in the reflection that we have found another (h,k,l) that contributes to it
						tempReflections[braggIndex].addKVectors(1);
					}
				}
			}
		}

		// Renumber reflections in BraggReflection array, assigning an index only if there are KVectors associated with it
		braggIndex = 0;
		for (int n=0; n<nBraggBins; ++n) if (tempReflections[n].nKVectors() > 0) tempReflections[n].setIndex(braggIndex++);

		// Collapse KVectors into a linear list, excluding any that weren't initialised
		KVector* kVectorsLinear = tempKVectors.linearArray();
		for (int n=0; n< tempKVectors.linearArraySize(); ++n)
		{
			if (kVectorsLinear[n].braggReflectionIndex() == -1) continue;

			// Look up and set the new index of the associated BraggReflection
			kVectorsLinear[n].setBraggReflectionIndex(tempReflections[kVectorsLinear[n].braggReflectionIndex()].index());
			braggKVectors.add(kVectorsLinear[n]);
		}

		// Prune BraggReflections array, putting them into a sequential Array that will reflect their new indexing
		for (int n=0; n<nBraggBins; ++n)
		{
			if (tempReflections[n].nKVectors() == 0) continue;
			braggReflections.add(tempReflections[n]);
		}

		Messenger::print("Bragg calculation spans %i k-vectors (max HKL = %i x %i x %i) over %f <= Q <= %f (%s elapsed).\n", braggKVectors.nItems(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, qMin, qMax, timer.elapsedTimeString());
		Messenger::print("%i unique Bragg reflections found using a Q resolution of %f Angstroms**-1.\n", braggReflections.nItems(), qDelta);

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
	Messenger::print("Calculated atomic cos/sin terms (%s elapsed)\n", timer.totalTimeString());

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
	Messenger::print("Calculated atomic contributions to k-vectors (%s elapsed)\n", timer.totalTimeString());

	// Zero Bragg reflection intensities
	BraggReflection* reflections = braggReflections.array();
	int nReflections = braggReflections.nItems();
	for (m=0; m<nReflections; ++m) reflections[m].reset();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggReflection
	for (m = 0; m < nKVectors; ++m) kVectors[m].calculateIntensities(reflections);

	// Normalise intensities against number of atoms and unit cell multiplicity
	AtomTypeData* atd1 = cfg->usedAtomTypes();
	for (int i = 0; i<cfg->nUsedAtomTypes(); ++i, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (int j = i; j<cfg->nUsedAtomTypes(); ++j, atd2 = atd2->next)
		{
			for (m=0; m<nReflections; ++m) reflections[m].scaleIntensity(i, j, 1.0 / (nAtoms * multiplicity.x * multiplicity.y * multiplicity.z));
		}
	}

	// Store the new version of the data
	GenericListHelper<int>::realise(cfg->moduleData(), "BraggVersion", "") = cfg->contentsVersion();

	return true;
}

// Form partial and total reflection functions from calculated reflection data
bool BraggModule::formReflectionFunctions(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax)
{
	// Retrieve BraggReflection data from the Configuration's module data
	bool found = false;
	const Array<BraggReflection>& braggReflections = GenericListHelper< Array<BraggReflection> >::value(cfg->moduleData(), "BraggReflections", "", Array<BraggReflection>(), &found);
	if (!found) return Messenger::error("Failed to find BraggReflection array in module data for Configuration '%s'.\n", cfg->name());
	const int nReflections = braggReflections.nItems();

	// Realise / retrieve storage for the Bragg partial S(Q) and combined F(Q)
	const int nTypes = cfg->nUsedAtomTypes();
	bool wasCreated;
	Array2D<Data1D>& braggPartials = GenericListHelper< Array2D<Data1D> >::realise(cfg->moduleData(), "OriginalBragg", "", GenericItem::InRestartFileFlag, &wasCreated);
	if (wasCreated)
	{
		// Create the triangular array
		braggPartials.initialise(nTypes, nTypes, true);
	
		// Generate empty Data1D over the Q range specified, setting bin centres
		Data1D temp;
		double q = 0.5*qDelta;
		while (q <= qMax)
		{
			temp.addPoint(q, 0.0);
			q += qDelta;
		}

		// Set up Data1D array with our empty data
		for (int n=0; n<braggPartials.linearArraySize(); ++n) braggPartials.linearArray()[n] = temp;
	}
	Data1D& braggTotal = GenericListHelper<Data1D>::realise(cfg->moduleData(), "OriginalBraggTotal", "", GenericItem::InRestartFileFlag, &wasCreated);
	if (wasCreated) braggTotal.setObjectTag(CharString("%s//OriginalBragg//Total", cfg->niceName()));
	braggTotal.clear();

	// Zero Bragg partials
	for (int n=0; n<braggPartials.linearArraySize(); ++n) braggPartials.linearArray()[n].values() = 0.0;

	// Loop over pairs of atom types, adding in contributions from our calculated BraggReflections
	double qCentre, inten, factor;
	int bin;
	AtomTypeData* atd1 = cfg->usedAtomTypes();
	for (int typeI = 0; typeI < nTypes; ++typeI, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (int typeJ = typeI; typeJ < nTypes; ++typeJ, atd2 = atd2->next)
		{
			// Retrieve partial container and make sure its object tag is set
			Data1D& partial = braggPartials.at(typeI, typeJ);
			partial.setObjectTag(CharString("%s//OriginalBragg//%s-%s", cfg->niceName(), atd1->atomTypeName(), atd2->atomTypeName()));

			// Loop over defined Bragg reflections
			for (int n=0; n<nReflections; ++n)
			{
				// Get q value and intensity of reflection
				qCentre = braggReflections.constAt(n).q();
				bin = qCentre / qDelta;
				inten = braggReflections.constAt(n).intensity(typeI, typeJ);

				// Account for doubling of partials between unlike atom types
				factor = (typeI == typeJ ? 1.0 : 2.0);

				partial.value(bin) += inten * factor;
			}

			// Add this partial into the total function
			braggTotal += partial;
		}
	}

	return true;
}

// Re-bin reflection data into supplied arrays
bool BraggModule::reBinReflections(ProcessPool& procPool, Configuration* cfg, Array2D<Data1D>& braggPartials)
{
	// Retrieve BraggReflection data from the Configuration's module data
	bool found = false;
	const Array<BraggReflection>& braggReflections = GenericListHelper< Array<BraggReflection> >::value(cfg->moduleData(), "BraggReflections", "", Array<BraggReflection>(), &found);
	if (!found) return Messenger::error("Failed to find BraggReflection array in module data for Configuration '%s'.\n", cfg->name());
	const int nReflections = braggReflections.nItems();

	const int nTypes = cfg->nUsedAtomTypes();

	// Create a temporary Data1D into which we will generate individual Bragg peak contributions
	const double qDelta = braggPartials.at(0,0).xAxis(1) - braggPartials.at(0,0).xAxis(0);
	const int nBins = braggPartials.at(0,0).nValues();
	Array<int> nAdded(nBins);

	nAdded = 0;

	// Loop over defined Bragg reflections
	// TODO Prune reflections based on intensity to speed-up process?
	int bin;
	for (int n=0; n<nReflections; ++n)
	{
		// Get Q bin (in the braggPartials) of the reflection
		bin = braggReflections.constAt(n).q() / qDelta;
		if ((bin < 0) || (bin >= nBins))
		{
			Messenger::warn("Reflection %i is at Q = %f Angstroms**-1, which is outside of the current Q range.\n", n, braggReflections.constAt(n).q());
			continue;
		}

		++nAdded[bin];

		// Loop over pairs of atom types, binning intensity contributions from this reflection
		AtomTypeData* atd1 = cfg->usedAtomTypes();
		for (int typeI = 0; typeI < nTypes; ++typeI, atd1 = atd1->next)
		{
			AtomTypeData* atd2 = atd1;
			for (int typeJ = typeI; typeJ < nTypes; ++typeJ, atd2 = atd2->next)
			{
				braggPartials.at(typeI, typeJ).value(bin) += braggReflections.constAt(n).intensity(typeI, typeJ);
			}
		}
	}

	// Normalise data against number of intensities added to each bin
	for (int typeI = 0; typeI < nTypes; ++typeI)
	{
		for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
		{
			for (int n=0; n<nBins; ++n) if (nAdded[n] > 0) braggPartials.at(typeI, typeJ).value(n) /= nAdded[n];
		}
	}
	return true;
}
