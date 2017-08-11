/*
	*** Partials Module - Functions
	*** src/modules/partials/functions.cpp
	Copyright T. Youngs 2012-2017

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

#include "modules/partials/partials.h"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggpeak.h"
#include "classes/cell.h"
#include "classes/kvector.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "templates/array3d.h"

// Partial Calculation Method enum
const char* PartialsMethodKeywords[] = { "Auto", "Test", "Simple", "Cells" };

// Convert character string to PartialsMethod
PartialsModule::PartialsMethod PartialsModule::partialsMethod(const char* s)
{
	for (int n=0; n<nPartialsMethods; ++n) if (DUQSys::sameString(s, PartialsMethodKeywords[n])) return (PartialsModule::PartialsMethod) n;
	return PartialsModule::nPartialsMethods;
}

// Return character string for PartialsMethod
const char* PartialsModule::partialsMethod(PartialsModule::PartialsMethod pm)
{
	return PartialsMethodKeywords[pm];
}

// Weighting Type enum
const char* WeightingTypeKeywords[] = { "None", "Neutron" };

// Convert character string to WeightingType
PartialsModule::WeightingType PartialsModule::weightingType(const char* s)
{
	for (int n=0; n<nWeightingTypes; ++n) if (DUQSys::sameString(s, WeightingTypeKeywords[n])) return (PartialsModule::WeightingType) n;
	return PartialsModule::nWeightingTypes;
}

// Return character string for WeightingType
const char* PartialsModule::weightingType(PartialsModule::WeightingType wt)
{
	return WeightingTypeKeywords[wt];
}

// Test supplied PartialSets against each other
bool PartialsModule::testReferencePartials(PartialSet& setA, PartialSet& setB, double testThreshold)
{
	// Get a copy of the AtomTypeList to work from
	AtomTypeList atomTypes = setA.atomTypes();
	AtomTypeData* typeI = atomTypes.first();
	double error;
	for (int n=0; n<atomTypes.nItems(); ++n, typeI = typeI->next)
	{
		AtomTypeData* typeJ = typeI;
		for (int m = n; m <atomTypes.nItems(); ++m, typeJ = typeJ->next)
		{
			// Full partial
			error = setA.partial(n,m).error(setB.partial(n,m));
			{
				Messenger::print("Partials: Test reference full partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Bound partial
			error = setA.boundPartial(n,m).error(setB.boundPartial(n,m));
			{
				Messenger::print("Partials: Test reference bound partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Unbound reference
			error = setA.unboundPartial(n,m).error(setB.unboundPartial(n,m));
			{
				Messenger::print("Partials: Test reference unbound partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Bragg reference
			error = setA.braggPartial(n,m).error(setB.braggPartial(n,m));
			{
				Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}
		}
	}

	// Total reference data supplied?
	error = setA.total().error(setB.total());
	{
		Messenger::print("Partials: Test reference total has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
		if (error > testThreshold) return false;
	}

	return true;
}

// Test reference data against calculated partials set
bool PartialsModule::testReferencePartials(GenericList& sourceModuleData, PartialSet& partials, const char* dataPrefix, double testThreshold)
{
	// We'll do a loop over all possible named test datasets that we expect to find with the given prefix and all combinations of atom type names.

	CharString dataName;

	// Get a copy of the AtomTypeList to work from
	AtomTypeList atomTypes = partials.atomTypes();
	AtomTypeData* typeI = atomTypes.first();
	for (int n=0; n<atomTypes.nItems(); ++n, typeI = typeI->next)
	{
		AtomTypeData* typeJ = typeI;
		for (int m = n; m <atomTypes.nItems(); ++m, typeJ = typeJ->next)
		{
			// Full partial (bound + unbound) reference data supplied?
			dataName = CharString("%s-%s-%s", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, uniqueName()))
			{
				double error = partials.partial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, uniqueName()));
				{
					Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Bound partial reference data supplied?
			dataName = CharString("%s-%s-%s-bound", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, uniqueName()))
			{
				double error = partials.boundPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, uniqueName()));
				{
					Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Unbound reference data supplied?
			dataName = CharString("%s-%s-%s-unbound", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, uniqueName()))
			{
				double error = partials.unboundPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, uniqueName()));
				{
					Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Bragg reference data supplied?
			dataName = CharString("%s-%s-%s-bragg", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, uniqueName()))
			{
				double error = partials.braggPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, uniqueName()));
				{
					Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}
		}
	}

	// Total reference data supplied?
	dataName = CharString("%s-total", dataPrefix);
	if (sourceModuleData.contains(dataName, uniqueName()))
	{
		double error = partials.total().error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, uniqueName()));
		{
			Messenger::print("Partials: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
			if (error > testThreshold) return false;
		}
	}

	return true;
}

// Calculate partial RDFs in serial with simple double-loop
bool PartialsModule::calculateTest(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet)
{
	// Calculate radial distribution functions with a simple double loop, in serial
	const Box* box = cfg->box();
	Atom* atoms = cfg->atoms();
	int ii, jj, typeI;
	double distance;
	double rbin = 1.0 / cfg->rdfBinWidth();
	Vec3<double> rI;

	for (ii = 0; ii < cfg->nAtoms()-1; ++ii)
	{
		rI = atoms[ii].r();
		typeI = atoms[ii].localTypeIndex();
		for (jj = ii+1; jj < cfg->nAtoms(); ++jj)
		{
			distance = box->minimumDistance(rI, atoms[jj].r());
			partialSet.fullHistogram(typeI, atoms[jj].localTypeIndex()).add(distance);
		}
	}

	return true;
}

// Calculate partial RDFs with optimised double-loop
bool PartialsModule::calculateSimple(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet)
{
	// Variables
	int n, m, nTypes, typeI, typeJ, i, j, nPoints;

	// Construct local arrays of atom type positions
	Messenger::printVerbose("Constructing local partial working arrays for %i types.\n", nTypes);
	nTypes = partialSet.nAtomTypes();
	const Box* box = cfg->box();
	Vec3<double>* r[nTypes];
	int maxr[nTypes], nr[nTypes];
	int* binss[nTypes], *bins;

	n = 0;
	for (AtomTypeData* atd = cfg->usedAtomTypes(); atd != NULL; atd = atd->next)
	{
		maxr[n] = atd->population();
		nr[n] = 0;
		r[n] = new Vec3<double>[maxr[n]];
		binss[n] = new int[maxr[n]];
		++n;
	}

	// Loop over Atoms and construct arrays
	Atom* atoms = cfg->atoms();
	for (n=0; n< cfg->nAtoms(); ++n)
	{
		m = atoms[n].localTypeIndex();
		r[m][nr[m]++] = atoms[n].r();
	}

	Messenger::printVerbose("Ready..\n");

	// Loop over assigned Atoms
	Vec3<double> centre, *ri, *rj, mim;
	int* histogram;
	double rbin = 1.0 / cfg->rdfBinWidth();

	// Loop context is to use all processes in Pool as one group
	int start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
	int stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);

	Messenger::printVerbose("Self terms..\n");
	
	// Self terms
	for (typeI = 0; typeI<nTypes; ++typeI)
	{
		ri = r[typeI];
		histogram = partialSet.fullHistogram(typeI,typeI).histogram().array();
		bins = binss[typeI];
		nPoints = partialSet.fullHistogram(typeI,typeI).nBins();
		for (i=start; i < maxr[typeI]; i += stride)
		{
			centre = ri[i];
			for (j = i+1; j < maxr[typeI]; ++j) bins[j] = box->minimumDistance(centre, ri[j]) * rbin;
			for (j = i+1; j < maxr[typeI]; ++j) if (bins[j] < nPoints) ++histogram[bins[j]];
		}
	}

	Messenger::printVerbose("Cross terms..\n");

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
			histogram = partialSet.fullHistogram(typeI,typeJ).histogram().array();
			bins = binss[typeJ];
			nPoints = partialSet.fullHistogram(typeI,typeJ).nBins();
			for (i=start; i < maxr[typeI]; i += stride)
			{
				centre = ri[i];
				for (j = 0; j < maxr[typeJ]; ++j) bins[j] = box->minimumDistance(centre, rj[j]) * rbin;
				for (j = 0; j < maxr[typeJ]; ++j) if (bins[j] < nPoints) ++histogram[bins[j]];
			}
		}
	}

	// Delete temporary arrays
	for (int n=0; n<nTypes; ++n)
	{
		delete[] r[n];
		delete[] binss[n];
	}

	return true;
}

// Calculate partial RDFs utilising Cell neighbour lists
bool PartialsModule::calculateCells(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet)
{
	Atom* i, *j;
	int n, m, ii, jj, nI, nJ, typeI, typeJ;
	Atom** atomsI, **atomsJ;
	Cell* cellI, *cellJ;
	double distance, rdfRange = cfg->rdfRange();
	Vec3<double> rI;

	// Grab the Box pointer and Cell array
	const Box* box = cfg->box();
	CellArray& cellArray = cfg->cells();

	for (n = 0; n<cellArray.nCells(); ++n)
	{
		cellI = cellArray.cell(n);
		atomsI = cellI->atoms().objects();
		nI = cellI->nAtoms();

		// Add contributions between atoms in cellI
		for (ii = 0; ii < nI-1; ++ii)
		{
			i = atomsI[ii];
			typeI = i->localTypeIndex();

			for (jj = ii+1; jj < nI; ++jj)
			{
				j = atomsI[jj];

				// No need to perform MIM since we're in the same cell
				distance = (i->r() - j->r()).magnitude();
				partialSet.fullHistogram(typeI,j->localTypeIndex()).add(distance);
			}
		}

		// Add contributions between atoms in cellI and cellJ
		for (m = n+1; m<cellArray.nCells(); ++m)
		{
			// Grab cell J and check that we need to consider it (i.e. it is within range)
			cellJ = cellArray.cell(m);
			if (!cellArray.withinRange(cellI, cellJ, rdfRange)) continue;

			atomsJ = cellJ->atoms().objects();
			nJ = cellJ->nAtoms();

			// Only perform mim on atom pairs if we really need to...
			if (cellArray.useMim(cellI,cellJ))
			{
				for (ii = 0; ii < nI; ++ii)
				{
					i = atomsI[ii];
					typeI = i->localTypeIndex();
					rI = i->r();

					for (jj = 0; jj < nJ; ++jj)
					{
						j = atomsJ[jj];
						distance = box->minimumDistance(j, rI);
						partialSet.fullHistogram(typeI, j->localTypeIndex()).add(distance);
					}
				}
			}
			else
			{
				for (ii = 0; ii < nI; ++ii)
				{
					i = atomsI[ii];
					typeI = i->localTypeIndex();
					rI = i->r();

					for (jj = 0; jj < nJ; ++jj)
					{
						j = atomsJ[jj];
						distance = (rI - j->r()).magnitude();
						partialSet.fullHistogram(typeI, j->localTypeIndex()).add(distance);
					}
				}
			}
		}
	}

	return true;
}

// Calculate unweighted partials for the specified Configuration
bool PartialsModule::calculateUnweightedGR(ProcessPool& procPool, Configuration* cfg, PartialsModule::PartialsMethod method, bool allIntra, int smoothing)
{
	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialSet& partialgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR", "Partials", GenericItem::NoFlag, &wasCreated);
	if (wasCreated) partialgr.setup(cfg, cfg->niceName(), "unweighted", "rdf", "r, Angstroms");

	// Is the PartialSet already up-to-date?
	// If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
	if ((partialgr.index() == cfg->coordinateIndex()) && (method != PartialsModule::TestMethod))
	{
		Messenger::print("Partials: Unweighted partial g(r) are up-to-date for Configuration '%s'.\n", cfg->name());
		return true;
	}

	Messenger::print("Partials: Calculating partial g(r) for Configuration '%s'...\n", cfg->name());

	/*
	 * Reset any existing data
	 */

	partialgr.reset();

	/*
	 * Calculate full (intra+inter) partials
	 */

	Timer timer;
	timer.start();
	procPool.resetAccumulatedTime();
	if (method == PartialsModule::TestMethod) calculateTest(procPool, cfg, partialgr);
	else if (method == PartialsModule::SimpleMethod) calculateSimple(procPool, cfg, partialgr);
	else if (method == PartialsModule::CellsMethod) calculateCells(procPool, cfg, partialgr);
	else if (method == PartialsModule::AutoMethod)
	{
		cfg->nAtoms() > 10000 ? calculateCells(procPool, cfg, partialgr) : calculateSimple(procPool, cfg, partialgr);
	}
	timer.stop();
	Messenger::print("Partials: Finished calculation of partials (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Calculate intramolecular partials
	 */

	double distance;
	int start, stride;
	const Box* box = cfg->box();
	CellArray& cellArray = cfg->cells();

	// Set start/stride for parallel loop (pool solo)
	start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
	stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);

	timer.start();
	// Loop over molecules...
	for (int m=start; m<cfg->nMolecules(); m += stride)
	{
		Molecule* mol = cfg->molecule(m);

		Atom* i, *j, *k;

		// Consider all intramolecular pairs?
		if (allIntra)
		{
			Atom** atoms = mol->atoms();
			for (int ii=0; ii<mol->nAtoms()-1; ++ii)
			{
				i = atoms[ii];
				for (int jj=ii+1; jj<mol->nAtoms(); ++jj)
				{
					j = atoms[jj];

					if (cellArray.useMim(i->cell(), j->cell())) distance = box->minimumDistance(i, j);
					else distance = (i->r() - j->r()).magnitude();
					partialgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).add(distance);
				}
			}
		}
		else
		{
			// Bonds
			for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
			{
				i = mol->atom(b->indexI());
				j = mol->atom(b->indexJ());
				if (cellArray.useMim(i->cell(), j->cell())) distance = box->minimumDistance(i, j);
				else distance = (i->r() - j->r()).magnitude();
				partialgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).add(distance);
			}

			// Angles
			for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next)
			{
				i = mol->atom(a->indexI());
				j = mol->atom(a->indexJ());
				k = mol->atom(a->indexK());
				
				// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
				if (cellArray.useMim(i->cell(), k->cell())) distance = box->minimumDistance(i, k);
				else distance = (i->r() - k->r()).magnitude();
				partialgr.boundHistogram(i->localTypeIndex(), k->localTypeIndex()).add(distance);
			}
		}
	}
	timer.stop();
	Messenger::print("Partials: Finished calculation of intramolecular partials (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Sum hstogram data
	 */

	// Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix knows
	// that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.

	int typeI, typeJ;
	procPool.resetAccumulatedTime();
	timer.start();
	for (typeI=0; typeI<partialgr.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<partialgr.nAtomTypes(); ++typeJ)
		{
			// Sum histogram data from all processes
			if (!partialgr.fullHistogram(typeI, typeJ).allSum(procPool)) return false;
			if (!partialgr.boundHistogram(typeI, typeJ).allSum(procPool)) return false;

			// Create unbound histogram from total and bound data
			partialgr.unboundHistogram(typeI, typeJ) = partialgr.fullHistogram(typeI, typeJ);
			partialgr.unboundHistogram(typeI, typeJ).addHistogramData(partialgr.boundHistogram(typeI, typeJ), -1.0);
		}
	}

	// Transform histogram data into radial ditribution functions
	XYData boxNorm = cfg->boxNormalisation();
	partialgr.formPartials(box->volume(), boxNorm);

	// Smooth partials if requested
	if (smoothing > 0)
	{
		for (typeI=0; typeI<partialgr.nAtomTypes(); ++typeI)
		{
			for (typeJ=typeI; typeJ<partialgr.nAtomTypes(); ++typeJ)
			{
				partialgr.partial(typeI,typeJ).smooth(smoothing*2+1);
				partialgr.boundPartial(typeI,typeJ).smooth(smoothing*2+1);
				partialgr.unboundPartial(typeI,typeJ).smooth(smoothing*2+1);
			}
		}
	}

	// Sum total functions
	partialgr.formTotal(true);
	timer.stop();
	Messenger::print("Partials: Finished summation and normalisation of partial g(r) data (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Partials are now up-to-date
	 */

	partialgr.setIndex(cfg->coordinateIndex());
	++staticLogPoint_;

	return true;
}

// Calculate weighted partials from supplied unweighted partials
bool PartialsModule::calculateWeightedGR(PartialSet& unweightedgr, PartialSet& weightedgr, Weights& weights)
{
	int typeI, typeJ;
	for (typeI=0; typeI<weightedgr.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<weightedgr.nAtomTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weights.fullWeight(typeI, typeJ);

			weightedgr.partial(typeI, typeJ).copyData(unweightedgr.partial(typeI, typeJ));
			weightedgr.partial(typeI, typeJ).arrayY() *= factor;
			weightedgr.boundPartial(typeI, typeJ).copyData(unweightedgr.boundPartial(typeI, typeJ));
			weightedgr.boundPartial(typeI, typeJ).arrayY() *= factor;
			weightedgr.unboundPartial(typeI, typeJ).copyData(unweightedgr.unboundPartial(typeI, typeJ));
			weightedgr.unboundPartial(typeI, typeJ).arrayY() *= factor;
			weightedgr.braggPartial(typeI, typeJ).copyData(unweightedgr.braggPartial(typeI, typeJ));
			weightedgr.braggPartial(typeI, typeJ).arrayY() *= factor;
		}
	}

	// Calculate total
	weightedgr.formTotal(false);

	return true;
}

// 	// Perform FT of reference data (if present)
// 	if (referenceData_.nPoints() > 0)
// 	{
// 		referenceDataFT_ = referenceData_;
// // 		referenceDataFT_.rebin();
// 		referenceDataFT_.transformLorch(rho, 0.025, 50.0, 0.5, 0.08, true);
// 	}


// Generate S(Q) from supplied g(r)
bool PartialsModule::calculateUnweightedSQ(ProcessPool& procPool, Configuration* cfg, double qMin, double qDelta, double qMax, double rho, XYData::WindowFunction wf, double qDepBroadening, double qIndepBroadening, bool braggOn, double qDepBraggBroadening, double qIndepBraggBroadening)
{
	// Grab unweighted g(r) for Configuration
	if (!cfg->moduleData().contains("UnweightedGR", "Partials"))
	{
		Messenger::error("Unweighted g(r) do not exist in Configuration '%s', so unweighted S(Q) cannot be calculated.\n", cfg->name());
		return false;
	}
	PartialSet& partialgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");

	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialSet& partialsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedSQ", "Partials", GenericItem::NoFlag, &wasCreated);
	if (wasCreated) partialsq.setup(partialgr.atomTypes(), cfg->niceName(), "unweighted", "sq", "Q, 1/Angstroms");

	// Is the PartialSet already up-to-date?
	if (partialsq.index() == cfg->coordinateIndex())
	{
		Messenger::print("Partials: Unweighted partial S(Q) are up-to-date for Configuration '%s'.\n", cfg->name());
		return true;
	}

	// Copy partial g(r) into our new S(Q) matrices
	Messenger::printVerbose("  --> Copying partial g(r) into S(Q) matrices...\n");
	int nTypes = partialgr.nAtomTypes();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			// Copy g(r) data into our arrays
			// Subtract 1.0 from the full and unbound partials so as to give (g(r)-1)
			// Don't subtract 1.0 from the bound partials, since they do not tend to 1.0 at longer r??
			partialsq.partial(n,m).copyData(partialgr.partial(n,m));
			partialsq.partial(n,m).arrayY() -= 1.0;
			partialsq.boundPartial(n,m).copyData(partialgr.boundPartial(n,m));
// 			partialsq.boundPartial(n,m).arrayY() -= 1.0;
			partialsq.unboundPartial(n,m).copyData(partialgr.unboundPartial(n,m));
			partialsq.unboundPartial(n,m).arrayY() -= 1.0;
		}
	}

	// Perform FT of partial g(r) into S(Q)
	// TODO Parallelise this
	procPool.resetAccumulatedTime();
	Timer timer;
	timer.start();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			if (!partialsq.partial(n,m).transformBroadenedRDF(rho, qMin, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;
			if (!partialsq.boundPartial(n,m).transformBroadenedRDF(rho, qMin, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;
			if (!partialsq.unboundPartial(n,m).transformBroadenedRDF(rho, qMin, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;

			// Zero Bragg partial, leave x array intact for use if needed
			partialsq.braggPartial(n,m).templateFrom(partialsq.partial(n,m));
		}
	}

	// Calculate Bragg partials (if requested)
	if (braggOn)
	{
		double braggQMax = cfg->braggQMax();
		double braggQMin = cfg->braggQMin();
		double mult = cfg->braggMultiplicity().x * cfg->braggMultiplicity().y * cfg->braggMultiplicity().z;
		Messenger::print("Partials: Bragg scattering will be calculated to Q = %f Angstroms**-1.\n", braggQMax);
		procPool.resetAccumulatedTime();
		timer.start();
		if (!calculateUnweightedBragg(procPool, cfg, partialsq, braggQMin, 0.01, braggQMax, qIndepBraggBroadening, qDepBroadening, mult)) return false;
		timer.stop();
		Messenger::print("Partials: Finished calculation of partial Bragg S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());
	}

	// Sum into total
	partialsq.formTotal(true);

	timer.stop();
	Messenger::print("Partials: Finished Fourier transform and summation of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * S(Q) are now up-to-date
	 */

	partialsq.setIndex(partialgr.index());

	return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q)
bool PartialsModule::calculateWeightedSQ(PartialSet& unweightedsq, PartialSet& weightedsq, Weights& weights)
{
	int typeI, typeJ;
	for (typeI=0; typeI<unweightedsq.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<unweightedsq.nAtomTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weights.fullWeight(typeI, typeJ);

			weightedsq.partial(typeI, typeJ).copyData(unweightedsq.partial(typeI, typeJ));
			weightedsq.partial(typeI, typeJ).arrayY() *= factor;
			weightedsq.boundPartial(typeI, typeJ).copyData(unweightedsq.boundPartial(typeI, typeJ));
			weightedsq.boundPartial(typeI, typeJ).arrayY() *= factor;
			weightedsq.unboundPartial(typeI, typeJ).copyData(unweightedsq.unboundPartial(typeI, typeJ));
			weightedsq.unboundPartial(typeI, typeJ).arrayY() *= factor;
			weightedsq.braggPartial(typeI, typeJ).copyData(unweightedsq.braggPartial(typeI, typeJ));
			weightedsq.braggPartial(typeI, typeJ).arrayY() *= factor;
		}
	}

	// Calculate total
	weightedsq.formTotal(false);

	return true;
}

// Calculate unweighted Bragg scattering for specified Configuration
bool PartialsModule::calculateUnweightedBragg(ProcessPool& procPool, Configuration* cfg, PartialSet& partialsq, double braggQMin, double braggQDelta, double braggQMax, double braggQIndepBroadening, double qDepBroadening, double braggMultiplicity)
{
	// Check to see if the arrays are up-to-date
	// TODO

	// Realise the arrays from the Configuration
	Array<KVector>& braggKVectors_ = GenericListHelper< Array<KVector> >::realise(cfg->moduleData(), "BraggKVectors", uniqueName());
	Array<BraggPeak>& braggPeaks_ = GenericListHelper< Array<BraggPeak> >::realise(cfg->moduleData(), "BraggPeaks", uniqueName());
	Array2D<double>& braggAtomVectorXCos_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXCos", uniqueName());
	Array2D<double>& braggAtomVectorYCos_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYCos", uniqueName());
	Array2D<double>& braggAtomVectorZCos_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZCos", uniqueName());
	Array2D<double>& braggAtomVectorXSin_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorXSin", uniqueName());
	Array2D<double>& braggAtomVectorYSin_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorYSin", uniqueName());
	Array2D<double>& braggAtomVectorZSin_ = GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "BraggAtomVectorZSin", uniqueName());
	Vec3<int>& braggMaximumHKL_ = GenericListHelper< Vec3<int> >::realise(cfg->moduleData(), "BraggMaximumHKL", uniqueName());

	// Grab some useful values
	const Box* box = cfg->box();
	Matrix3 rAxes = box->reciprocalAxes();
	Vec3<double> rI, v, rLengths = box->reciprocalAxisLengths();
	AtomTypeList atomTypes = partialsq.atomTypes();
	int nTypes = atomTypes.nItems();
	int nAtoms = cfg->nAtoms();
	Atom* atoms = cfg->atoms();

	int n, m, h, k, l, hAbs, kAbs, lAbs, typeI, typeJ;
	double* cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL, *cosTerms, *sinTerms;
	double hkCos, hkSin, hklCos, hklSin;

	// Create a timer
	Timer timer;

	rAxes.print();

	// Calculate number of k-vectors within cutoff range
	double mag, magSq;
	int braggIndex;
	timer.start();
	if (braggKVectors_.nItems() == 0)
	{
		Messenger::print("Partials: Performing initial setup of Bragg arrays...\n");
		timer.start();

		double braggQMaxSq = braggQMax*braggQMax, braggQMinSQ = braggQMin*braggQMin;
		int nBraggBins = braggQMax / braggQDelta + 1;

		// Determine extents of hkl indices to use
		braggMaximumHKL_.x = braggQMax / rLengths.x;
		braggMaximumHKL_.y = braggQMax / rLengths.y;
		braggMaximumHKL_.z = braggQMax / rLengths.z;

		// Clear old arrays
		braggPeaks_.clear();
		braggKVectors_.clear();

		// Create temporary 3D array for k-vectors, and linear array for Bragg peaks
		OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL_.x, -braggMaximumHKL_.y, braggMaximumHKL_.y, -braggMaximumHKL_.z, braggMaximumHKL_.z);
		Array<BraggPeak> tempPeaks(nBraggBins);

		// Initialise Bragg peaks
		// The Q value reflects the centre-bin value of the peak.
		double q = 0.5*braggQDelta;
		for (int n=0; n<nBraggBins; ++n)
		{
			tempPeaks[n].initialise(q, n, nTypes);
			q += braggQDelta;
		}
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
					if ((magSq >= braggQMinSQ) && (magSq <= braggQMaxSq))
					{
						mag = sqrt(magSq);

						// Calculate index of associated Bragg peak in the peaks array
						braggIndex = int(mag/braggQDelta);

						// Point this (h,k,l) value to this Bragg peak
						tempKVectors.ref(h, k, l).initialise(h, k, l, braggIndex, nTypes);

						// Note in the peak that we have found another (h,k,l) that contributes to it
						tempPeaks[braggIndex].addKVectors(1);
					}
				}
			}
		}

		// Collapse KVectors into a linear list, excluding any that weren't initialised
		KVector* kVectorsLinear = tempKVectors.linearArray();
		for (int n=0; n< tempKVectors.linearArraySize(); ++n)
		{
			if (kVectorsLinear[n].braggPeakIndex() == -1) continue;
			braggKVectors_.add(kVectorsLinear[n]);
		}

		// Prune BraggPeaks array
		for (int n=0; n<nBraggBins; ++n)
		{
			if (tempPeaks[n].nKVectors() == 0) continue;
			braggPeaks_.add(tempPeaks[n]);
		}

		Messenger::print("Partials: Bragg calculation spans %i hkl indices (max HKL = %i x %i x %i) over %f <= Q <= %f (%s elapsed).\n", braggKVectors_.nItems(), braggMaximumHKL_.x, braggMaximumHKL_.y, braggMaximumHKL_.z, braggQMin, braggQMax, timer.elapsedTimeString());
		Messenger::print("Partials: %i unique Bragg peaks found (qDelta = %f Angstroms**-1)\n", braggPeaks_.nItems(), braggQDelta);

		// Create atom working arrays
		braggAtomVectorXCos_.initialise(nAtoms, braggMaximumHKL_.x+1);
		braggAtomVectorYCos_.initialise(nAtoms, braggMaximumHKL_.y+1);
		braggAtomVectorZCos_.initialise(nAtoms, braggMaximumHKL_.z+1);
		braggAtomVectorXSin_.initialise(nAtoms, 2*braggMaximumHKL_.x+1);
		braggAtomVectorYSin_.initialise(nAtoms, 2*braggMaximumHKL_.y+1);
		braggAtomVectorZSin_.initialise(nAtoms, 2*braggMaximumHKL_.z+1);
	}

	// Precalculate cos/sin terms for atoms
	timer.stop();
	timer.zero();
	timer.start();
	for (n = 0; n<nAtoms; ++n)
	{
		// Calculate reciprocal lattice atom coordinates
		v = atoms[n].r();
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
	Messenger::print("Partials: Calculated atomic cos/sin terms (%s elapsed)\n", timer.totalTimeString());

	// Calculate k-vector contributions
	KVector* kVectors = braggKVectors_.array();
	const int nKVectors = braggKVectors_.nItems();
	int localTypeIndex;

	// Zero kvector cos/sin contributions
	for (m = 0; m < nKVectors; ++m) kVectors[m].zeroCosSinTerms();

	// Loop over atoms
	timer.start();
	for (n = 0; n<nAtoms; ++n)
	{
	// 		printf("n = %i\n, natoms = %i\n", n, nAtoms);
		// Grab localTypeIndex and array pointers for this atom
		localTypeIndex = atoms[n].localTypeIndex();
		cosTermsH = braggAtomVectorXCos_.ptr(n, 0);
		cosTermsK = braggAtomVectorYCos_.ptr(n, 0);
		cosTermsL = braggAtomVectorZCos_.ptr(n, 0);
		sinTermsH = braggAtomVectorXSin_.ptr(n, braggMaximumHKL_.x);
		sinTermsK = braggAtomVectorYSin_.ptr(n, braggMaximumHKL_.y);
		sinTermsL = braggAtomVectorZSin_.ptr(n, braggMaximumHKL_.z);

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
	BraggPeak* peaks = braggPeaks_.array();
	int nPeaks = braggPeaks_.nItems();
	for (m=0; m<nPeaks; ++m) peaks[m].resetIntensities();

	// Calculate intensities for individual KVectors - this will be automatically summed into the corresponding BraggPeak
	for (m = 0; m < nKVectors; ++m) kVectors[m].calculateIntensities(peaks);

	double lFactor = cfg->box()->reciprocalVolume() / (PI*PI);
	double qCentre, factor, lambda, lambdaCubed, inten, qSub, qAdd, broaden, q;

	// Construct a working XYData in which to broaden and sum Bragg peaks - use braggQDelta as the binwidth
	XYData tempBraggSQ;
	q = 0.5*braggQDelta;
	while (q <= braggQMax)
	{
		tempBraggSQ.addPoint(q, 0.0);
		q += braggQDelta;
	}

	// Loop over pairs of atom types, constructing Bragg partial SQ for each
	for (int typeI = 0; typeI < nTypes; ++typeI)
	{
		for (int typeJ = typeI; typeJ < nTypes; ++typeJ)
		{
			tempBraggSQ.arrayY() = 0.0;

			// Calculate prefactor
			factor = (typeI == typeJ ? 1.0 : 2.0);

			// Loop over defined Bragg peaks
			for (n=0; n<nPeaks; ++n)
			{
				// Get q value and intensity of peak
				qCentre = peaks[n].q();
				inten = peaks[n].intensity(typeI, typeJ);
				lambda = braggQIndepBroadening + qCentre * qDepBroadening;
				lambdaCubed = lambda * lambda * lambda;
				tempBraggSQ.addY(qCentre/braggQDelta, inten);

				// Loop over points in braggSQ XYData (which will provide our x-values)
// 				for (m=0; m<braggSQ.nPoints(); ++m)
// 				{
// 					// Get q value from array
// 					q = braggSQ.x(m);
// 
// 					// Set up Lorentzian parameters
// 					qSub = (qCentre - q) / lambda;
// 					qAdd = (qCentre + q) / lambda;
// 					broaden = lFactor / ((1.0 + qSub*qSub) * (1.0 + qAdd*qAdd) * lambdaCubed);
// 					printf("y = %f   %f\n", braggSQ.y(m), inten*broaden);
// 					braggSQ.addY(m, inten * broaden);
// 				}
			}

			// Normalise to total number of atoms and the Bragg multiplicity
			tempBraggSQ.arrayY() /= nAtoms * braggMultiplicity;
			// Normalise to atomic fractions
			tempBraggSQ.arrayY() *= atomTypes[typeI]->fraction() * atomTypes[typeJ]->fraction();

			// Add data into the PartialSet, rebinning on to its local Q scale
			partialsq.braggPartial(typeI, typeJ).addInterpolated(tempBraggSQ);

		}
	}

	for (m=0; m<nPeaks; ++m) Messenger::print("  %f   %f\n", peaks[m].q(), peaks[m].intensity(0,0));

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

