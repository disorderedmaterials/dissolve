/*
	*** RDF Module - Functions
	*** src/modules/rdf/functions.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/rdf/rdf.h"
#include "main/duq.h"
#include "module/group.h"
#include "classes/atomtype.h"
#include "classes/configuration.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "templates/genericlisthelper.h"
#include "templates/orderedpointerarray.h"

/*
 * Private Functions
 */

// Calculate partial g(r) in serial with simple double-loop
bool RDFModule::calculateGRTestSerial(Configuration* cfg, PartialSet& partialSet)
{
	// Calculate radial distribution functions with a simple double loop, in serial
	const Box* box = cfg->box();
	Atom** atoms = cfg->atoms().array();
	int ii, jj, typeI;
	double distance;
	double rbin = 1.0 / cfg->rdfBinWidth();
	Vec3<double> rI;

	for (ii = 0; ii < cfg->nAtoms()-1; ++ii)
	{
		rI = atoms[ii]->r();
		typeI = atoms[ii]->localTypeIndex();
		for (jj = ii+1; jj < cfg->nAtoms(); ++jj)
		{
			distance = box->minimumDistance(rI, atoms[jj]->r());
			partialSet.fullHistogram(typeI, atoms[jj]->localTypeIndex()).add(distance);
		}
	}

	return true;
}

// Calculate partial g(r) with optimised double-loop
bool RDFModule::calculateGRSimple(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet)
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
	Atom** atoms = cfg->atoms().array();
	for (n=0; n< cfg->nAtoms(); ++n)
	{
		m = atoms[n]->localTypeIndex();
		r[m][nr[m]++] = atoms[n]->r();
	}

	Messenger::printVerbose("Ready..\n");

	// Loop over assigned Atoms
	Vec3<double> centre, *ri, *rj, mim;
	int* histogram;
	double rbin = 1.0 / cfg->rdfBinWidth();

	// Loop context is to use all processes in Pool as one group
	int start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
	int stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

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

// Calculate partial g(r) utilising Cell neighbour lists
bool RDFModule::calculateGRCells(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet)
{
	Atom* i, *j;
	int n, m, ii, jj, nI, nJ, typeI, typeJ;
	Cell* cellI, *cellJ;
	double distance, rdfRange = cfg->rdfRange();
	Vec3<double> rI;

	// Grab the Box pointer and Cell array
	const Box* box = cfg->box();
	CellArray& cellArray = cfg->cells();

	// Loop context is to use all processes in Pool as one group
	int start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
	int stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

	for (n = start; n<cellArray.nCells(); n += stride)
	{
		cellI = cellArray.cell(n);
		OrderedPointerArray<Atom>& atomsI = cellI->atoms();
		nI = atomsI.nItems();

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

			OrderedPointerArray<Atom>& atomsJ = cellJ->atoms();
			nJ = atomsJ.nItems();

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

// Perform averaging of specifed partials
bool RDFModule::performGRAveraging(GenericList& moduleData, const char* name, const char* prefix, int nSetsInAverage, RDFModule::AveragingScheme averagingScheme)
{
	const int expDecay = 0.7;

	// Find the 'root' PartialSet, which should currently contain the most recently-calculated data
	if (!moduleData.contains(name, prefix))
	{
		Messenger::error("Couldn't find root PartialSet '%s' (prefix = '%s') in order to perform averaging.\n", name, prefix);
		return false;
	}
	PartialSet& currentPartials = GenericListHelper<PartialSet>::retrieve(moduleData, name, prefix);

	// Store the current fingerprint, since we must ensure we retain it in the averaged PartialSet.
	CharString currentFingerprint = currentPartials.fingerprint();

	// Establish how many stored datasets we have
	int nStored = 0;
	for (nStored = 0; nStored < nSetsInAverage; ++nStored) if (!moduleData.contains(CharString("%s_%i", name, nStored+1), prefix)) break;
	Messenger::print("Average requested over %i datsets - %i available in module data (%i max).\n", nSetsInAverage, nStored, nSetsInAverage-1);

	// If a stored PartialSet with index 1 exists, we need to check its index value against the current one.
	// If it is the same, we do not store it since it is the same and does not add any new information to the averaging.
	bool storeCurrent = true;
	if (moduleData.contains(CharString("%s_1", name), prefix))
	{
		PartialSet& lastPartials = GenericListHelper<PartialSet>::retrieve(moduleData, CharString("%s_1", name), prefix);
		if (DUQSys::sameString(lastPartials.fingerprint(), currentFingerprint, true))
		{
			Messenger::print("Current partials will not form part of average, since they are the same as the last stored set.\n");
			storeCurrent = false;
		}
	}

	// So, store the current PartialSet?
	if (storeCurrent)
	{
		// Remove the oldest dataset if it exists, and shuffle the others down
		if (nStored == nSetsInAverage)
		{
			moduleData.remove(CharString("%s_%i", name, nStored), prefix);
			--nStored;
		}
		for (int n=nStored; n>0; --n)
		{
			// Rename the module data
			moduleData.rename(CharString("%s_%i", name, n), prefix, CharString("%s_%i", name, n+1), prefix);

			// Grab it, and rename the resources within
			PartialSet& previousPartials = GenericListHelper<PartialSet>::retrieve(moduleData, CharString("%s_%i", name, n+1), prefix);
			previousPartials.setObjectNames(currentPartials.objectNamePrefix(), CharString("Avg%i", n+1));
		}

		// Store the current PartialSet as the earliest data (1)
		PartialSet& recentPartials = GenericListHelper<PartialSet>::realise(moduleData, CharString("%s_1", name), prefix, GenericItem::InRestartFileFlag);
		recentPartials = currentPartials;
		recentPartials.setObjectNames(currentPartials.objectNamePrefix(), "Avg1");
		++nStored;
	}

	// Calculate normalisation
	double normalisation = 0.0;
	if (averagingScheme == RDFModule::SimpleAveraging) normalisation = nStored;
	else if (averagingScheme == RDFModule::ExponentialAveraging) normalisation = (1.0 - pow(expDecay,nStored)) / (1.0 - expDecay); 

	// Perform averaging of the datsets that we have
	currentPartials.reset();
	double weight = 1.0;
	for (int n=0; n<nStored; ++n)
	{
		// Get the (n+1)'th dataset
		PartialSet& set = GenericListHelper<PartialSet>::retrieve(moduleData, CharString("%s_%i", name, n+1), prefix);

		// Determine the weighting factor
		if (averagingScheme == RDFModule::SimpleAveraging) weight = 1.0 / normalisation;
		else if (averagingScheme == RDFModule::ExponentialAveraging) weight = pow(expDecay, n) / normalisation;

		// Sum in partials
		currentPartials.addPartials(set, weight);
	}

	// Reinstate the index of the averaged PartialSet
	currentPartials.setFingerprint(currentFingerprint);

	return true;
}

/*
 * Public Functions
 */

// Calculate unweighted partials for the specified Configuration
bool RDFModule::calculateGR(ProcessPool& procPool, Configuration* cfg, RDFModule::PartialsMethod method, bool allIntra, bool& alreadyUpToDate)
{
	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialSet& originalgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "OriginalGR", "", GenericItem::InRestartFileFlag, &wasCreated);
	if (wasCreated) originalgr.setUp(cfg, cfg->niceName(), "original", "rdf", "r, Angstroms");

	// Is the PartialSet already up-to-date?
	// If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
	alreadyUpToDate = false;
	if (DUQSys::sameString(originalgr.fingerprint(), CharString("%i", cfg->coordinateIndex())) && (method != RDFModule::TestMethod))
	{
		Messenger::print("Partial g(r) are up-to-date for Configuration '%s'.\n", cfg->name());
		alreadyUpToDate = true;
		return true;
	}

	Messenger::print("Calculating partial g(r) for Configuration '%s'...\n", cfg->name());

	/*
	 * Make sure histograms are set up, and reset any existing data
	 */

	originalgr.setUpHistograms(cfg->rdfRange(), cfg->rdfBinWidth());
	originalgr.reset();

	/*
	 * Calculate full (intra+inter) partials
	 */

	Timer timer;
	timer.start();
	procPool.resetAccumulatedTime();
	if (method == RDFModule::TestMethod) calculateGRTestSerial(cfg, originalgr);
	else if (method == RDFModule::SimpleMethod) calculateGRSimple(procPool, cfg, originalgr);
	else if (method == RDFModule::CellsMethod) calculateGRCells(procPool, cfg, originalgr);
	else if (method == RDFModule::AutoMethod)
	{
		cfg->nAtoms() > 10000 ? calculateGRCells(procPool, cfg, originalgr) : calculateGRSimple(procPool, cfg, originalgr);
	}
	timer.stop();
	Messenger::print("RDF: Finished calculation of partials (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Calculate intramolecular partials
	 */

	double distance;
	const Box* box = cfg->box();
	CellArray& cellArray = cfg->cells();

	// Set start/stride for parallel loop (pool solo)
	int start = (method == RDFModule::TestMethod ? 0 : procPool.interleavedLoopStart(ProcessPool::PoolStrategy));
	int stride = (method == RDFModule::TestMethod ? 1 : procPool.interleavedLoopStride(ProcessPool::PoolStrategy));

	timer.start();

	// If we're considering the intrmolecular part to be between all pairs, as opposed to just those that explicitly bound,
	// perform a loop over molecules. Otherwise, use the Configuration's master lists of intramolecular terms.
	Atom* i, *j, *k;
	if (allIntra)
	{
		// Loop over molecules...
		for (int m=start; m<cfg->nMolecules(); m += stride)
		{
			Molecule* mol = cfg->molecule(m);
			Atom** atoms = mol->atoms();

			for (int ii=0; ii<mol->nAtoms()-1; ++ii)
			{
				i = atoms[ii];
				for (int jj=ii+1; jj<mol->nAtoms(); ++jj)
				{
					j = atoms[jj];

					if (cellArray.useMim(i->cell(), j->cell())) distance = box->minimumDistance(i, j);
					else distance = (i->r() - j->r()).magnitude();
					originalgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).add(distance);
				}
			}
		}
	}
	else
	{
		// Bonds
		Bond** bonds = cfg->bonds().array();
		Bond* b;
		for (int n=start; n<cfg->nBonds(); n+=stride)
		{
			b = bonds[n];

			i = b->i();
			j = b->j();
			if (cellArray.useMim(i->cell(), j->cell())) distance = box->minimumDistance(i, j);
			else distance = (i->r() - j->r()).magnitude();
			originalgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).add(distance);
		}

		// Angles
		Angle** angles = cfg->angles().array();
		Angle* a;
		for (int n=start; n<cfg->nAngles(); n+=stride)
		{
			a = angles[n];

			i = a->i();
			j = a->j();
			k = a->k();
			
			// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
			if (cellArray.useMim(i->cell(), k->cell())) distance = box->minimumDistance(i, k);
			else distance = (i->r() - k->r()).magnitude();
			originalgr.boundHistogram(i->localTypeIndex(), k->localTypeIndex()).add(distance);
		}
	}
	timer.stop();
	Messenger::print("RDF: Finished calculation of intramolecular partials (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Sum histogram data
	 */

	// Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix knows
	// that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.

	int typeI, typeJ;
	procPool.resetAccumulatedTime();
	timer.start();
	for (typeI=0; typeI<originalgr.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<originalgr.nAtomTypes(); ++typeJ)
		{
			// Sum histogram data from all processes (except if using RDFModule::TestMethod, where all processes have all data already)
			if (method != RDFModule::TestMethod)
			{
				if (!originalgr.fullHistogram(typeI, typeJ).allSum(procPool)) return false;
				if (!originalgr.boundHistogram(typeI, typeJ).allSum(procPool)) return false;
			}

			// Create unbound histogram from total and bound data
			originalgr.unboundHistogram(typeI, typeJ) = originalgr.fullHistogram(typeI, typeJ);
			originalgr.unboundHistogram(typeI, typeJ).addHistogramData(originalgr.boundHistogram(typeI, typeJ), -1.0);
		}
	}

	// Transform histogram data into radial distribution functions
	XYData boxNorm = cfg->boxNormalisation();
	originalgr.formPartials(box->volume(), boxNorm);

	// Sum total functions
	originalgr.formTotal(true);
	timer.stop();
	Messenger::print("RDF: Finished summation and normalisation of partial g(r) data (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * Partials are now up-to-date
	 */
	
	originalgr.setFingerprint(CharString("%i", cfg->coordinateIndex()));
	++staticLogPoint_;

	return true;
}

// Calculate smoothed/broadened partial g(r) from supplied partials
bool RDFModule::calculateUnweightedGR(PartialSet& originalgr, PartialSet& unweightedgr, BroadeningFunction intraBroadening, int smoothing)
{
	// Copy data
	unweightedgr = originalgr;

	int typeI, typeJ;
	for (typeI=0; typeI<unweightedgr.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<unweightedgr.nAtomTypes(); ++typeJ)
		{
			// Copy data from the unweighted g(r) provided, and broaden / smooth as requested
			unweightedgr.partial(typeI, typeJ).copyData(originalgr.partial(typeI, typeJ));
			unweightedgr.boundPartial(typeI, typeJ).copyData(originalgr.boundPartial(typeI, typeJ));
			unweightedgr.unboundPartial(typeI, typeJ).copyData(originalgr.unboundPartial(typeI, typeJ));
			unweightedgr.braggPartial(typeI, typeJ).copyData(originalgr.braggPartial(typeI, typeJ));

			// Broaden bound partials?
			if (intraBroadening.function() != BroadeningFunction::NoFunction)
			{
				// Remove bound part from full partial
				unweightedgr.partial(typeI, typeJ).addInterpolated(unweightedgr.boundPartial(typeI, typeJ), -1.0);

				// Convolute the bound partial with the broadening function
				unweightedgr.boundPartial(typeI, typeJ).convolute(intraBroadening);

				// Add the broadened bound partial back into the full partial array
				unweightedgr.partial(typeI, typeJ).addInterpolated(unweightedgr.boundPartial(typeI, typeJ), 1.0);
			}

			// Smooth partials if requested
			if (smoothing > 0)
			{
				unweightedgr.partial(typeI,typeJ).movingAverage(smoothing);
				unweightedgr.boundPartial(typeI,typeJ).movingAverage(smoothing);
				unweightedgr.unboundPartial(typeI,typeJ).movingAverage(smoothing);
			}
		}
	}

	// Calculate total
	unweightedgr.formTotal(false);

	return true;
}

// Return effective density for specified Module's target Configurations
double RDFModule::summedRho(Module* module, GenericList& processingModuleData)
{
	double rho0 = 0.0, totalWeight = 0.0;
	RefListIterator<Configuration,bool> targetIterator(module->targetConfigurations());
	while (Configuration* cfg = targetIterator.iterate())
	{
		double weight = GenericListHelper<double>::retrieve(processingModuleData, CharString("Weight_%s", cfg->niceName()), module->uniqueName(), 1.0);
		totalWeight += weight;

		rho0 += weight / cfg->atomicDensity();
	}
	rho0 /= totalWeight;
	rho0 = 1.0 / rho0;

	return rho0;
}

// Sum unweighted g(r) over the supplied Module's target Configurations
bool RDFModule::sumUnweightedGR(ProcessPool& procPool, Module* module, GenericList& processingModuleData, PartialSet& summedUnweightedGR)
{
	// Create an AtomTypeList containing the sum of atom types over all target configurations
	AtomTypeList combinedAtomTypes;
	RefListIterator<Configuration,bool> targetIterator(module->targetConfigurations());
	while (Configuration* cfg = targetIterator.iterate()) combinedAtomTypes.add(cfg->usedAtomTypesList());

	// Finalise and print the combined AtomTypes matrix
	combinedAtomTypes.finalise();

	// Set up PartialSet container
	summedUnweightedGR.setUpPartials(combinedAtomTypes, module->uniqueName(), "unweighted", "gr", "r, Angstroms");
	summedUnweightedGR.setObjectNames(CharString("%s//UnweightedGR", module->uniqueName()));

	// Determine total weighting factors and combined density over all Configurations, and set up a Configuration/weight RefList for simplicity
	RefList<Configuration,double> configWeights;
	targetIterator.restart();
	double totalWeight = 0.0;
	while (Configuration* cfg = targetIterator.iterate())
	{
		// Get weighting factor for this Configuration to contribute to the summed partials
		double weight = GenericListHelper<double>::retrieve(processingModuleData, CharString("Weight_%s", cfg->niceName()), module->uniqueName(), 1.0);
		Messenger::print("Weight for Configuration '%s' is %f.\n", cfg->name(), weight);
	
		// Add our Configuration target
		configWeights.add(cfg, weight);
		totalWeight += weight;
	}

	// Calculate overall density of combined system
	double rho0 = 0.0;
	RefListIterator<Configuration,double> weightsIterator(configWeights);
	while (Configuration* cfg = weightsIterator.iterate()) rho0 += (weightsIterator.currentData() / totalWeight) / cfg->atomicDensity();
	rho0 = 1.0 / rho0;

	// Sum Configurations into the PartialSet
	CharString fingerprint;
	weightsIterator.restart();
	while (Configuration* cfg = weightsIterator.iterate())
	{
		// Update fingerprint
		fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->coordinateIndex()) : CharString("_%i", cfg->coordinateIndex());

		// Calculate weighting factor
		double weight = ((weightsIterator.currentData() / totalWeight) * cfg->atomicDensity()) / rho0;

		// Grab partials for Configuration and add into our set
		if (!cfg->moduleData().contains("UnweightedGR")) return Messenger::error("Couldn't find UnweightedGR data for Configuration '%s'.\n", cfg->name());
		PartialSet& cfgPartialGR = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR");
		summedUnweightedGR.addPartials(cfgPartialGR, weight);
	}
	summedUnweightedGR.setFingerprint(fingerprint);

	// Store the overall density of our partials
	GenericListHelper<double>::realise(processingModuleData, "EffectiveRho", module->uniqueName(), GenericItem::InRestartFileFlag) = rho0;

	return true;
}

// Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
bool RDFModule::sumUnweightedGR(ProcessPool& procPool, Module* parentModule, ModuleGroup* moduleGroup, GenericList& processingModuleData, PartialSet& summedUnweightedGR)
{
	// Determine total weighting factor over all Configurations, and set up a Configuration/weight RefList for simplicity
	RefList<Configuration,double> configWeights;
	double totalWeight = 0.0;
	RefListIterator<Module,bool> moduleIterator(moduleGroup->modules());
	while (Module* module = moduleIterator.iterate())
	{
		// Loop over Configurations defined in this target
		RefListIterator<Configuration,bool> targetIterator(module->targetConfigurations());
		while (Configuration* cfg = targetIterator.iterate())
		{
			// Get weighting factor for this Configuration to contribute to the summed partials
			double weight = GenericListHelper<double>::retrieve(processingModuleData, CharString("Weight_%s", cfg->niceName()), module->uniqueName(), 1.0);
			Messenger::print("Weight for Configuration '%s' is %f.\n", cfg->name(), weight);
		
			// Add our Configuration target
			configWeights.add(cfg, weight);
			totalWeight += weight;
		}
	}
	Messenger::print("Total weight over all Configurations for summed unweighted g(r) is %f (%i Configurations)\n", totalWeight, configWeights.nItems());

	// Calculate overall density of combined system, normalising the Configuration weights as we go, and create an AtomTypeList to cover all used types
	double rho0 = 0.0;
	AtomTypeList combinedAtomTypes;
	RefListIterator<Configuration,double> weightsIterator(configWeights);
	while (Configuration* cfg = weightsIterator.iterate())
	{
		weightsIterator.currentData() /= totalWeight;
		rho0 += weightsIterator.currentData() / cfg->atomicDensity();

		combinedAtomTypes.add(cfg->usedAtomTypesList());
	}
	rho0 = 1.0 / rho0;
	Messenger::print("Effective density for summed unweighted g(r) over group is %f\n", rho0);

	// Finalise the combined AtomTypes matrix
	combinedAtomTypes.finalise();

	// Set up PartialSet container
	summedUnweightedGR.setUpPartials(combinedAtomTypes, parentModule->uniqueName(), "unweighted", "gr", "r, Angstroms");
	summedUnweightedGR.setObjectNames(CharString("%s//UnweightedGR//%s", parentModule->uniqueName(), moduleGroup->name()));

	// Sum Configurations into the PartialSet
	CharString fingerprint;
	weightsIterator.restart();
	while (Configuration* cfg = weightsIterator.iterate())
	{
		// Update fingerprint
		fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->coordinateIndex()) : CharString("_%i", cfg->coordinateIndex());

		// Calculate weighting factor
		double weight = (weightsIterator.currentData() * cfg->atomicDensity()) / rho0;

		// *Copy* the partials for the Configuration, subtract 1.0, and add into our set
		if (!cfg->moduleData().contains("UnweightedGR")) return Messenger::error("Couldn't find UnweightedGR data for Configuration '%s'.\n", cfg->name());
		PartialSet cfgPartialGR = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR");
		cfgPartialGR -= 1.0;
		summedUnweightedGR.addPartials(cfgPartialGR, weight);
	}
	summedUnweightedGR.setFingerprint(fingerprint);
	summedUnweightedGR += 1.0;

	// Store the overall density of our partials
// 	GenericListHelper<double>::realise(moduleData, "EffectiveRho", module->uniqueName(), GenericItem::InRestartFileFlag) = rho0;

	return true;
}

// Test supplied PartialSets against each other
bool RDFModule::testReferencePartials(PartialSet& setA, PartialSet& setB, double testThreshold)
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
				Messenger::print("RDF: Test reference full partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Bound partial
			error = setA.boundPartial(n,m).error(setB.boundPartial(n,m));
			{
				Messenger::print("RDF: Test reference bound partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Unbound reference
			error = setA.unboundPartial(n,m).error(setB.unboundPartial(n,m));
			{
				Messenger::print("RDF: Test reference unbound partial '%s-%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}

			// Bragg reference
			error = setA.braggPartial(n,m).error(setB.braggPartial(n,m));
			{
				Messenger::print("RDF: Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", typeI->atomTypeName(), typeJ->atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}
		}
	}

	// Total reference data supplied?
	error = setA.total().error(setB.total());
	{
		Messenger::print("RDF: Test reference total has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
		if (error > testThreshold) return false;
	}

	return true;
}

// Test reference data against calculated partials set
bool RDFModule::testReferencePartials(GenericList& sourceModuleData, const char* sourceModuleUniqueName, PartialSet& partials, const char* dataPrefix, double testThreshold)
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
			if (sourceModuleData.contains(dataName, sourceModuleUniqueName))
			{
				double error = partials.partial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, sourceModuleUniqueName));
				{
					Messenger::print("Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Bound partial reference data supplied?
			dataName = CharString("%s-%s-%s-bound", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, sourceModuleUniqueName))
			{
				double error = partials.boundPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, sourceModuleUniqueName));
				{
					Messenger::print("Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Unbound reference data supplied?
			dataName = CharString("%s-%s-%s-unbound", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, sourceModuleUniqueName))
			{
				double error = partials.unboundPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, sourceModuleUniqueName));
				{
					Messenger::print("Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}

			// Bragg reference data supplied?
			dataName = CharString("%s-%s-%s-bragg", dataPrefix, typeI->atomTypeName(), typeJ->atomTypeName());
			if (sourceModuleData.contains(dataName, sourceModuleUniqueName))
			{
				double error = partials.braggPartial(n,m).error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, sourceModuleUniqueName));
				{
					Messenger::print("Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
					if (error > testThreshold) return false;
				}
			}
		}
	}

	// Total reference data supplied?
	dataName = CharString("%s-total", dataPrefix);
	if (sourceModuleData.contains(dataName, sourceModuleUniqueName))
	{
		double error = partials.total().error(GenericListHelper<XYData>::retrieve(sourceModuleData, dataName, sourceModuleUniqueName));
		{
			Messenger::print("Test reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", dataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
			if (error > testThreshold) return false;
		}
	}

	return true;
}
