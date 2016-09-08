/*
	*** Configuration - Partials Data
	*** src/classes/configuration_partials.cpp
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

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/comms.h"

// Calculate full partial RDFs using simple double-loop over atoms
bool Configuration::calculatePartialsSimple()
{
	msg.print("Calculating partial RDFs (simple double-loop)...\n");

	// Variables
	int n, m, nTypes, typeI, typeJ, i, j, nPoints;

	// Construct local arrays of atom type positions
	msg.printVerbose("Constructing local arrays.\n");
	nTypes = usedAtomTypes_.nItems();
	Vec3<double>* r[nTypes];
	int maxr[nTypes], nr[nTypes];
	int* binss[nTypes], *bins;
	n = 0;
	for (AtomTypeData* atd = usedAtomTypes_.first(); atd != NULL; atd = atd->next)
	{
		maxr[n] = atd->population();
		nr[n] = 0;
		r[n] = new Vec3<double>[maxr[n]];
		binss[n] = new int[maxr[n]];
		++n;
	}

	// Loop over Atoms and construct arrays
	for (n=0; n<nAtoms_; ++n)
	{
		m = atoms_[n].localTypeIndex();
		r[m][nr[m]++] = atoms_[n].r();
	}

	msg.printVerbose("Ready..\n");

	// Loop over assigned Atoms
	Vec3<double> centre, *ri, *rj, mim;
	int* histogram;
	double rbin = 1.0 / rdfBinWidth_;

	int start = Comm.interleavedLoopStart(DUQComm::World);
	int stride = Comm.interleavedLoopStride(DUQComm::World);
	
	// Self terms
	for (typeI = 0; typeI<nTypes; ++typeI)
	{
		ri = r[typeI];
		histogram = pairRDFMatrix_.ref(typeI,typeI).histogram();
		bins = binss[typeI];
		nPoints = pairRDFMatrix_.ref(typeI,typeI).nBins();
		for (i=start; i < maxr[typeI]; i += stride)
		{
			centre = ri[i];
			for (j = i+1; j < maxr[typeI]; ++j) bins[j] = box_->minimumDistance(centre, ri[j]) * rbin;
			for (j = i+1; j < maxr[typeI]; ++j) if (bins[j] < nPoints) ++histogram[bins[j]];
		}
// 		printf("For types %i-%i, count = %i\n", typeI, typeI, count);
	}

	// Cross terms
	for (typeI = 0; typeI<nTypes; ++typeI)
	{
		ri = r[typeI];
		for (typeJ = 0; typeJ<nTypes; ++typeJ)
		{
			// Skip if typeI == typeJ, or if the number of atoms in typeI is greater than typeJ (since it is less efficient)
			if (typeI == typeJ) continue;
			if (nr[typeI] > nr[typeJ]) continue;
			if ((nr[typeI] == nr[typeJ]) && (typeI > typeJ)) continue;

			rj = r[typeJ];
			histogram = pairRDFMatrix_.ref(typeI,typeJ).histogram();
			bins = binss[typeJ];
			nPoints = pairRDFMatrix_.ref(typeI,typeJ).nBins();
			for (i=start; i < maxr[typeI]; i += stride)
			{
				centre = ri[i];
				for (j = 0; j < maxr[typeJ]; ++j) bins[j] = box_->minimumDistance(centre, rj[j]) * rbin;
				for (j = 0; j < maxr[typeJ]; ++j) if (bins[j] < nPoints) ++histogram[bins[j]];
			}
// 			printf("For types %i-%i count = %i, time = %s\n", typeI, typeJ, count, timer.timeString());
		}
	}

	return true;
}

// Calculate intramolecular RDFs
bool Configuration::calculateIntramolecularRDFs()
{
	// Calculate intramolecular RDFs
	double distance;
	int start, stride;

	// Set start/skip for parallel loop
	start = Comm.interleavedLoopStart(DUQComm::World);
	stride = Comm.interleavedLoopStride(DUQComm::World);

	// Loop over molecules...
	int n;
	Atom* i, *j, *k;
	for (int m=start; m<nMolecules(); m += stride)
	{
		Molecule* mol = molecule(m);

		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			i = mol->atom(b->indexI());
			j = mol->atom(b->indexJ());
			if (useMim(i->cell(), j->cell())) distance = box_->minimumDistance(i, j);
			else distance = (i->r() - j->r()).magnitude();
			boundRDFMatrix_.ref(i->localTypeIndex(), j->localTypeIndex()).add(distance);
		}

		// Angles
		for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
		{
			i = mol->atom(a->indexI());
			j = mol->atom(a->indexJ());
			k = mol->atom(a->indexK());
			
			// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
			if (useMim(i->grain()->cell(), k->grain()->cell())) distance = box_->minimumDistance(i, k);
			else distance = (i->r() - k->r()).magnitude();
			boundRDFMatrix_.ref(i->localTypeIndex(), k->localTypeIndex()).add(distance);
		}
	}

	return true;
}

// Calculate Bragg S(Q)
bool Configuration::calculateBraggSQ()
{
	// Grab some useful values
	Matrix3 rAxes = box_->reciprocalAxes();
	Vec3<double> rI, v, rLengths = box_->reciprocalAxisLengths();
	int nTypes = usedAtomTypes_.nItems();

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
	double mag, magSq, braggMaxSq = braggMaximumQ_*braggMaximumQ_;
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
					if ((h == 0) && (k == 0) && (l == 0)) continue;

					kVec.z = l;
					v = rAxes * kVec;

					// Calculate magnitude of this k vector
					magSq = v.magnitudeSq();
					if (magSq <= braggMaxSq)
					{
						mag = sqrt(magSq);

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
		braggAtomVectorXCos_.initialise(nAtoms_, braggMaximumHKL_.x+1);
		braggAtomVectorYCos_.initialise(nAtoms_, braggMaximumHKL_.y+1);
		braggAtomVectorZCos_.initialise(nAtoms_, braggMaximumHKL_.z+1);
		braggAtomVectorXSin_.initialise(nAtoms_, 2*braggMaximumHKL_.x+1);
		braggAtomVectorYSin_.initialise(nAtoms_, 2*braggMaximumHKL_.y+1);
		braggAtomVectorZSin_.initialise(nAtoms_, 2*braggMaximumHKL_.z+1);
	}

	// Precalculate cos/sin terms for atoms
	Comm.resetAccumulatedTime();
	timer.stop();
	for (n = 0; n<nAtoms_; ++n)
	{
		// Calculate reciprocal lattice atom coordinates
		v = atoms_[n].r();
		rI.x = v.x*rAxes[0] + v.y*rAxes[1] + v.z*rAxes[2];
		rI.y = v.x*rAxes[3] + v.y*rAxes[4] + v.z*rAxes[5];
		rI.z = v.x*rAxes[6] + v.y*rAxes[7] + v.z*rAxes[8];

		// Grab pointers to cos/sin arrays for atom
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
	int localTypeIndex;

	// Zero kvector cos/sin contributions
	for (m = 0; m < nKVectors; ++m) kVectors[m]->zeroCosSinTerms();

	// Loop over atoms
	Comm.resetAccumulatedTime();
	timer.start();
	for (n = 0; n<nAtoms_; ++n)
	{
		// Grab localTypeIndex and array pointers for this atom
		localTypeIndex = atoms_[n].localTypeIndex();
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
			kVector->addCosTerm(localTypeIndex, hklCos);
			kVector->addSinTerm(localTypeIndex, hklSin);
		}
	}
	timer.stop();
	msg.print("--> Calculated atomic contributions to k-vectors (%s elapsed, %s comms)\n", timer.timeString(), Comm.accumulatedTimeString());

	// Zero Bragg peak intensities
	BraggPeak** peaks = braggPeaks_.objects();
	int nPeaks = braggPeaks_.nItems();
	for (m=0; m<nPeaks; ++m) peaks[m]->resetIntensities();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
	for (m = 0; m < nKVectors; ++m) kVectors[m]->calculateIntensities();

	// Create individual functions in braggSQMatrix_
	double factor, qCentre, q, inten, qSub, qAdd, broaden, lFactor, lambda, lambdaCubed;

	lFactor = box_->reciprocalVolume() / (PI*PI);

	for (typeI = 0; typeI < nTypes; ++typeI)
	{
		for (typeJ = typeI; typeJ < nTypes; ++typeJ)
		{
			Data2D& braggSQ = braggSQMatrix_.ref(typeI, typeJ);

			// Calculate prefactor
			factor = (typeI == typeJ ? 1.0 : 2.0);

			// Loop over defined Bragg peaks
			for (n=0; n<nPeaks; ++n)
			{
				// Get q value and intensity of peak
				qCentre = peaks[n]->q();
				inten = peaks[n]->intensity(typeI, typeJ);
				lambda = braggBroadening_ + qCentre * qDependentFWHM_;
				lambdaCubed = lambda * lambda * lambda;

				// Loop over points in braggSQ Data2D (which will provide our x-values)
				for (m=0; m<braggSQ.nPoints(); ++m)
				{
					// Get q value from array
					q = braggSQ.x(m);

					// Set up Lorentzian parameters
					qSub = (qCentre - q) / lambda;
					qAdd = (qCentre + q) / lambda;
					broaden = lFactor / ((1.0 + qSub*qSub) * (1.0 + qAdd*qAdd) * lambdaCubed);
					braggSQ.addY(m, inten * broaden);
				}
			}

			// Normalise to total number of atoms, subtract single atom scattering, and normalise to atomic fractions
			braggSQ.arrayY() /= nAtoms_;
			if (typeI == typeJ) braggSQ.arrayY() -= usedAtomTypes_[typeI]->fraction();
			braggSQ.arrayY() /= usedAtomTypes_[typeI]->fraction() * usedAtomTypes_[typeJ]->fraction();
		}
	}

// 	for (m=0; m<nPeaks; ++m) msg.print("  %f   %f\n", peaks[m]->q(), peaks[m]->intensity(0,0));

	return true;
}

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

// Set RDF calculation style
void Configuration::setRDFMethod(Configuration::RDFMethod method)
{
	rdfMethod_ = method;
}

// Return RDF calculation style
Configuration::RDFMethod Configuration::rdfMethod()
{
	return rdfMethod_;
}

// Set Q delta to use in generated S(Q)
void Configuration::setQDelta(double delta)
{
	qDelta_ = delta;
}

// Return Q delta to use in generated S(Q)
double Configuration::qDelta()
{
	return qDelta_;
}

// Set maximum Q to calculate S(Q) to
void Configuration::setQMax(double qMax)
{
	qMax_ = qMax;
}

// Return maximum Q to calculate S(Q) to
double Configuration::qMax()
{
	return qMax_;
}

// Set whether Bragg calculation is activated for this configuration
void Configuration::setBraggCalculationOn(bool on)
{
	braggCalculationOn_ = on;
}

// Return whether Bragg calculation is activated for this configuration
bool Configuration::braggCalculationOn()
{
	return braggCalculationOn_;
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

// Set broadening for Bragg features
void Configuration::setBraggBroadening(double broadening)
{
	braggBroadening_ = broadening;
}

// Return broadening for Bragg features
double Configuration::braggBroadening()
{
	return braggBroadening_;
}

// Set maximal extent of hkl for Bragg calculation
void Configuration::setBraggMaximumHKL(Vec3<int> hklMax)
{
}

// Return maximal extent of hkl for Bragg calculation
Vec3<int> Configuration::braggMaximumHKL()
{
	return braggMaximumHKL_;
}

/*!
 * \brief Setup RDF / F(Q) storage
 */
bool Configuration::setupPartials()
{
	// Construct a matrix based on the local usedAtomTypes_ list, since this reflects all our possible partials
	int n, m;
	int nTypes = usedAtomTypes_.nItems();

	msg.print("--> Creating matrices (%ix%i)...\n", nTypes, nTypes);

	pairRDFMatrix_.initialise(nTypes, nTypes, true);
	boundRDFMatrix_.initialise(nTypes, nTypes, true);
	unboundRDFMatrix_.initialise(nTypes, nTypes, true);
	pairSQMatrix_.initialise(nTypes, nTypes, true);
	boundSQMatrix_.initialise(nTypes, nTypes, true);
	braggSQMatrix_.initialise(nTypes, nTypes, true);
	partialSQMatrix_.initialise(nTypes, nTypes, true);
	unboundSQMatrix_.initialise(nTypes, nTypes, true);

	Dnchar title;
	AtomTypeData* at1 = usedAtomTypes_.first(), *at2;
	msg.print("--> Creating Lists of partials and linking into matrices...\n");
	msg.printVerbose("Range/binWidth/Volume = %f/%f/%f\n", rdfRange_, rdfBinWidth_, box_->volume());
	for (n=0; n<nTypes; ++n, at1 = at1->next)
	{
		at2 = at1;
		for (m=n; m<nTypes; ++m, at2 = at2->next)
		{
			// Partial g(r)
			title.sprintf("%s-%s", at1->name(), at2->name());
			pairRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			pairRDFMatrix_.ref(n,m).normalisedData().setName(title.get());
			boundRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			boundRDFMatrix_.ref(n,m).normalisedData().setName(title.get());
			unboundRDFMatrix_.ref(n,m).initialise(0.0, rdfRange_, rdfBinWidth_);
			unboundRDFMatrix_.ref(n,m).normalisedData().setName(title.get());

			// -- For normalisation, self-terms must be multiplied by 2.0
			pairRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(box_->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);
			boundRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(box_->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);
			unboundRDFMatrix_.ref(n,m).setRadialNumberDensityNormalisation(box_->volume(), at1->population(), at2->population(), at1 == at2 ? 2.0 : 1.0, boxNormalisation_);

			// Partial S(Q)
			pairSQMatrix_.ref(n,m).setName(title.get());
			boundSQMatrix_.ref(n,m).setName(title.get());
			unboundSQMatrix_.ref(n,m).setName(title.get());
			partialSQMatrix_.ref(n,m).setName(title.get());
			braggSQMatrix_.ref(n,m).setName(title.get());
			braggSQMatrix_.ref(n,m).createEmpty(qDelta_, braggMaximumQ_);
		}
	}

	// Total g(r)
	int nBins = pairRDFMatrix_.ref(0,0).nBins();
	totalRDF_.initialise(nBins);
	for (n=0; n<nBins; ++n) totalRDF_.setX(n, (n+0.5)*rdfBinWidth_);
	totalRDF_.setName("Unweighted");

	// Total F(Q)
	totalFQ_.setName("Unweighted");

	return true;
}

/*!
 * \brief Reset pair correlations
 */
void Configuration::resetPairCorrelations()
{
	for (int n=0; n<usedAtomTypes_.nItems(); ++n)
	{
		for (int m=n; m<usedAtomTypes_.nItems(); ++m)
		{
			pairRDFMatrix_.ref(n,m).reset();
			boundRDFMatrix_.ref(n,m).reset();
			unboundRDFMatrix_.ref(n,m).reset();
			pairSQMatrix_.ref(n,m).clear();
			boundSQMatrix_.ref(n,m).clear();
			unboundSQMatrix_.ref(n,m).clear();
			boundSQMatrix_.ref(n,m).clear();
			partialSQMatrix_.ref(n,m).clear();
			braggSQMatrix_.ref(n,m).arrayY() = 0.0;
		}
	}
	totalRDF_.arrayY() = 0.0;
	totalFQ_.arrayY() = 0.0;
}

/*!
 * \brief Calculate pair correlations
 */
bool Configuration::calculatePairCorrelations(Data2D::WindowFunction windowFunction)
{
	// Check that we actually need to calculate new partials
	if (coordinateIndex_ == partialsIndex_)
	{
		msg.printVerbose("Refused to calculate partials for configuration - nothing has changed since the last calculation.\n");
		return true;
	}

	// Clear old RDF/S(Q) data
	resetPairCorrelations();
	
	// Calculate full partials
	Timer timer;
	switch (rdfMethod_)
	{
		case (Configuration::SimpleMethod):
			if (!calculatePartialsSimple()) return false;
			break;
		default:
			msg.error("Partial calculation style unrecognised!\n");
			return false;
	}

	// Update partials index
	partialsIndex_ = coordinateIndex_;

	// Collect all processes together
	if (!Comm.wait(DUQComm::World)) return false;

	timer.stop();
	msg.print("--> Finished calculation of partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate intramolecular partials
	timer.start();
	if (!calculateIntramolecularRDFs()) return false;
	timer.stop();
	msg.print("--> Finished calculation of intramolecular partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Perform summation of partial data
	// Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix knows
	// that (i,j) == (j,i) as it is stored as a half-matrix in an Array2D object.
	int typeI, typeJ;
	Comm.resetAccumulatedTime();
	timer.start();
	double rho = atomicDensity();
	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			// Sum histogram data from all processes
			if (!pairRDFMatrix_.ref(typeI,typeJ).allSum()) return false;
			if (!boundRDFMatrix_.ref(typeI,typeJ).allSum()) return false;

			// Create unbound histogram from total and bound data
			unboundRDFMatrix_.ref(typeI, typeJ) = pairRDFMatrix_.ref(typeI,typeJ);
			unboundRDFMatrix_.ref(typeI, typeJ).addHistogramData(boundRDFMatrix_.ref(typeI,typeJ), -1.0);

			// Finalise (normalise) partials
			pairRDFMatrix_.ref(typeI,typeJ).finalise();
			boundRDFMatrix_.ref(typeI,typeJ).finalise();
			unboundRDFMatrix_.ref(typeI,typeJ).finalise();

			// Smooth partials if requested
			if (rdfSmoothing_ > 0)
			{
				pairRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
				boundRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
				unboundRDFMatrix_.ref(typeI,typeJ).normalisedData().smooth(rdfSmoothing_*2+1);
			}

			// Copy RDF data ready for Fourier transform
			// -- Copy RDF data
			pairSQMatrix_.ref(typeI,typeJ) = pairRDFMatrix_.ref(typeI,typeJ).normalisedData();
			pairSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
			boundSQMatrix_.ref(typeI,typeJ) = boundRDFMatrix_.ref(typeI,typeJ).normalisedData();
// 			boundSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
			unboundSQMatrix_.ref(typeI,typeJ) = unboundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			unboundSQMatrix_.ref(typeI,typeJ).arrayY() -= 1.0;
		}
	}
	timer.stop();
	msg.print("--> Finished summation and normalisation of partial RDF data (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Perform FT of partial g(r) into S(Q)
	// No instrumental broadening is applied in this case - the Configuration-based S(Q) are 'pure' in that sense
	// TODO Parallelise this
	Comm.resetAccumulatedTime();
	timer.start();
	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			if (!pairSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, qDelta_, qMax_, 0.0, 0.0, windowFunction)) return false;
			if (!boundSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, qDelta_, qMax_, 0.0, 0.0, windowFunction)) return false;
			if (!unboundSQMatrix_.ref(typeI,typeJ).transformBroadenedRDF(rho, qDelta_, qMax_, 0.0, 0.0, windowFunction)) return false;
		}
	}
	timer.stop();
	msg.print("--> Finished Fourier transform of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate Bragg partials (if requested)
	if (braggCalculationOn_)
	{
		Comm.resetAccumulatedTime();
		timer.start();
		if (!calculateBraggSQ()) return false;
		timer.stop();
		msg.print("--> Finished calculation of partial Bragg S(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());
	}

	// Generate final partial S(Q) combining pair correlations and Bragg partials
	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			// Grab references:
			// -- Full atom pair-pair structure factors (bound + unbound)
			Data2D& pairSQ = pairSQMatrix_.ref(typeI,typeJ);
			// -- Bragg partial structure factors
			Data2D& braggSQ = braggSQMatrix_.ref(typeI,typeJ);
			// -- Full partial structure factors (destination)
			Data2D& partialSQ = partialSQMatrix_.ref(typeI,typeJ);

			// Copy atomic pair S(Q) information
			partialSQ = pairSQ;

			// Combine Bragg(Q) data if it was calculated
			if (braggCalculationOn_)
			{
				double xRange = 0.1, x;
				double xMin = braggMaximumQ_ - xRange - qDelta_*0.5;
				for (int n=0; n<braggSQ.nPoints(); ++n)
				{
					x = (braggSQ.x(n) <= xMin ? 0.0 : (braggSQ.x(n) - xMin) / xRange);
					partialSQ.setY(n, pairSQ.y(n)*x + braggSQ.y(n)*(1.0-x));
					// TEST - Straight replacement of original pair data
// 					partialSQ.setY(n, braggSQ.y(n));
				}
			}
		}
	}

	// Calculate total unweighted g(r) and F(Q)
	totalFQ_ = pairSQMatrix_.ref(0,0);
	totalFQ_.arrayY() = 0.0;
	double factor, braggMax;
	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			factor = usedAtomTypes_[typeI]->fraction() * usedAtomTypes_[typeJ]->fraction() * (typeI == typeJ ? 1.0 : 2.0);

			// Total RDF
			totalRDF_.addY(pairRDFMatrix_.ref(typeI,typeJ).normalisedData().arrayY(), factor);
			// TODO Does not include contributions from Bragg partials

			// F(Q)
			totalFQ_.addY(partialSQMatrix_.ref(typeI,typeJ).arrayY(), factor);
		}
	}
	timer.stop();
	msg.print("--> Finished summation and FT of partials, and generation of total unweighted RDF/F(Q) (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	// Calculate weighted partials for each Sample, along with current RMSE
	Comm.resetAccumulatedTime();
	timer.start();
	totalRMSE_ = 0.0;
	msg.print("XXXX TODO Implement calculation of ReferenceData/Sample RDF/SQ calculation.\n");
// 	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
// 	{
// 		if (!sam->calculatePairCorrelations(pairRDFMatrix_, pairSQMatrix_, braggSQMatrix_, partialSQMatrix_)) return false;
// 		if (sam->hasReferenceData()) totalRMSE_ += sam->referenceRMSE(rmseDeltaQ_);
// 	}
	timer.stop();
	msg.print("--> Finished generation of Sample partials (%s elapsed, %s comms).\n", timer.timeString(), Comm.accumulatedTimeString());

	msg.print("--> Current RMSE over all Samples = %f barns/sr/atom\n", totalRMSE_);

	return true;
}

/*!
 * \brief Save all unweighted RDFs
 */
void Configuration::saveRDFs(const char* baseName)
{
	// Only the Master process can do this
	if (!Comm.master()) return;
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.rdf", baseName, usedAtomTypes_[typeI]->name(), usedAtomTypes_[typeJ]->name());

			// Open file and check that we're OK to proceed writing to it
			msg.print("Writing RDF file '%s'...\n", filename.get());

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				msg.error("Couldn't open file '%s' for writing.\n", filename.get());
				continue;
			}
			
			Data2D& rdf = pairRDFMatrix_.ref(typeI,typeJ).normalisedData();
			Data2D& bound = boundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			Data2D& unbound = unboundRDFMatrix_.ref(typeI,typeJ).normalisedData();
			parser.writeLineF("# %-14s  %-16s  %-16s  %-16s\n", "r, Angstroms", "g(r)", "bound(r)", "unbound(r)"); 
			for (n = 0; n<rdf.nPoints(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e\n", rdf.x(n), rdf.y(n), bound.y(n), unbound.y(n));
			parser.closeFiles();
		}
	}

	Dnchar filename(-1, "%s-unweighted-total.rdf", baseName);
	totalRDF_.save(filename);
}

/*!
 * \brief Save all partial S(Q)
 */
void Configuration::saveSQ(const char* baseName)
{
	// Only the Master process can do this
	if (!Comm.master()) return;
	LineParser parser;
	int typeI, typeJ, n;

	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
	{
		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
		{
			Dnchar filename(-1, "%s-unweighted-%s-%s.sq", baseName, usedAtomTypes_[typeI]->name(), usedAtomTypes_[typeJ]->name());

			// Open file and check that we're OK to proceed writing to it
			msg.print("Writing S(Q) file '%s'...\n", filename.get());

			parser.openOutput(filename, true);
			if (!parser.isFileGoodForWriting())
			{
				msg.error("Couldn't open file '%s' for writing.\n", filename.get());
				continue;
			}
			
			Data2D& pair = pairSQMatrix_.ref(typeI,typeJ);
			Data2D& bound = boundSQMatrix_.ref(typeI,typeJ);
			Data2D& unbound = unboundSQMatrix_.ref(typeI,typeJ);
			Data2D& bragg = braggSQMatrix_.ref(typeI,typeJ);
			Data2D& partial = partialSQMatrix_.ref(typeI,typeJ);
			parser.writeLineF("# Unweighted partial S(Q) for types %s and %s calculated on TODO\n", usedAtomTypes_[typeI]->name(), usedAtomTypes_[typeJ]->name());
			parser.writeLine("#       S(Q) contains full pair correlation (bound + unbound) and Bragg scattering (if calculated)\n");
			parser.writeLine("#   Bound(Q) contains only pair correlations between atoms joined by intramolecular bonds or angles\n");
			parser.writeLine("# Unbound(Q) contains only pair correlations between atoms *not* joined by intramolecular bonds or angles\n");
			parser.writeLine("#    Pair(Q) contains full pair correlations\n");
			parser.writeLine("#   Bragg(Q) contains correlations calculated from HKL indices\n");
			parser.writeLineF("# %-16s  %-16s  %-16s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "Bound(Q)", "Unbound(Q)", "Pair(Q)", "Bragg(Q)");
			for (n = 0; n< pair.nPoints(); ++n)
			{
				if (n < bragg.nPoints()) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e  %16.10e  %16.10e\n", partial.x(n), partial.y(n), bound.y(n), unbound.y(n), pair.y(n), bragg.y(n));
				else parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e  %16.10e\n", partial.x(n), partial.y(n), bound.y(n), unbound.y(n), pair.y(n));
			}
			parser.closeFiles();
		}
	}

	Dnchar filename(-1, "%s-unweighted-total.fq", baseName);
	totalFQ_.save(filename);
}
