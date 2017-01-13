/*
	*** Partials Module
	*** src/modules/partials.cpp
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
#include "modules/partials.h"
#include "classes/box.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "classes/weightsmatrix.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include "base/timer.h"

// Static Members
List<Module> Partials::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
Partials::Partials() : Module()
{
	// Add to instances list and set unique name for this instance
	uniqueName_.sprintf("%s_%02i", name(), instances_.nItems());
	instances_.own(this);

	// Setup variables / control parameters
	frequency_ = 5;
	options_.add("Save", bool(false), "Whether to save partials to disk after calculation");
	options_.add("Smoothing", 0, "Specifies the degree of smoothing 'n' to apply to calculated RDFs, where 2n+1 controls the length in the applied Spline smooth");
	options_.add("UseMixFrom", uniqueName_, "Unique Module name under which to search for Species/Isotopologue mix information");
	options_.add("Weights", "None", "Weighting scheme to use for calculated partials (None,Neutron)");
}

// Destructor
Partials::~Partials()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& Partials::instances()
{
	return instances_;
}

// Create instance of this module
Module* Partials::createInstance()
{
	return new Partials;
}

/*
 * Definition
 */

// Return name of module
const char* Partials::name()
{
	return "Partials";
}

// Return brief description of module
const char* Partials::brief()
{
	return "Calculate partial and total RDFs";
}

// Return instance type for module
Module::InstanceType Partials::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool Partials::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool Partials::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool Partials::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* Partials::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool Partials::setupDependentModule(Module* depMod)
{
	return true;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int Partials::nTargetableConfigurations()
{
	return -1;
}

// Return the maximum number of Samples the Module can target (or -1 for any number)
int Partials::nTargetableSamples()
{
	return 1;
}

/*
 * Method
 */

// Perform setup tasks for module
bool Partials::setup(ProcessPool& procPool)
{
	return false;
}

// Execute pre-processing stage
bool Partials::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool Partials::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate weighted or unweighted partials and total RDFs for the Sample or Configuration
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */

	// If there is a Sample target, then we calculate the weighted structure factors for it (using the supplied Configurations)
	// Otherwise take the Configuration targets and calculate unweighted structure factors for them.

	if (targetSamples_.nItems() > 0)
	{
		// Assemble partials from all Configurations specified, weighting them accordingly
		CharString varName;
		double totalWeight = 0.0;
		AtomTypeList atomTypes;

		// Get target Sample
		Sample* sam = targetSamples_.firstItem();

		// Retrieve control parameters from Configuration
		const bool saveData = GenericListHelper<bool>::retrieve(sam->moduleData(), "Save", uniqueName(), options_.valueAsBool("Save"));
		CharString mixSource = GenericListHelper<CharString>::retrieve(sam->moduleData(), "UseMixFrom", uniqueName_, options_.valueAsString("UseMixFrom"));
		CharString weightsString = GenericListHelper<CharString>::retrieve(sam->moduleData(), "Weights", uniqueName_, options_.valueAsString("Weights"));
		Partials::WeightingType weightsType = Partials::NoWeighting;
		if (DUQSys::sameString(weightsString, "Neutron")) weightsType = Partials::NeutronWeighting;
		else if (DUQSys::sameString(weightsString, "None"))
		{
			Messenger::error("Invalid weighting scheme found for Sample '%s'.\n", weightsString.get());
			return false;
		}

		// Print argument/parameter summary
		Messenger::print("Partials: Save data is %s.\n", DUQSys::onOff(saveData));
		Messenger::print("Partials: Isotopologue mixture data will be taken from Module '%s'.\n", mixSource.get());

		// Loop over Configurations, creating a list of unique AtomTypes encountered over all. Also, make sure the unweighted partials are up-to-date.
		RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
		while (Configuration* cfg = configIterator.iterate())
		{
			// Add any missing atom types into our local list
			atomTypes.add(cfg->usedAtomTypesList());

			// Calculate partials for Configuration
			calculateUnweighted(cfg, procPool);
			
			// Need to calculate weighted terms here if requested
			if (weightsType != Partials::NoWeighting)
			{
				// Construct weights matrix based on Isotopologue specifications in some module (specified by mixSource) and the populations of atomtypes in the Configuration
				WeightsMatrix weightsMatrix;
				RefListIterator<Species,double> speciesIterator(cfg->usedSpecies());
				while (Species* sp = speciesIterator.iterate())
				{
					int speciesPopulation = speciesIterator.currentData() * cfg->multiplier();

					// Loop over available Isotopologues for Species
					for (Isotopologue* availableIso = sp->isotopologues(); availableIso != NULL; availableIso = availableIso->next)
					{
						// Construct variable name that we expect to find if the tope was used in the Module (variable is defined in the associated Configuration)
						varName.sprintf("Isotopologue/%s/%s", sp->name(), availableIso->name());
						if (sam->moduleData().contains(varName, mixSource))
						{
							// This isotopologue is defined as being used, so add its (in the isotopic proportions defined in the Isotopologue) to the weightsMatrix.
							weightsMatrix.addIsotopologue(sp, speciesPopulation, availableIso, GenericListHelper<double>::retrieve(sam->moduleData(), varName, mixSource));
						}
					}
				}

				// We will complain strongly if a species in the Configuration is not covered by at least one Isotopologue definition
				speciesIterator.restart();
				while (Species* sp = speciesIterator.iterate()) if (!weightsMatrix.hasSpeciesIsotopologueMixture(sp)) 
				{
					Messenger::error("Isotopologue specification for Species '%s' in Configuration '%s' is missing.\n", sp->name(), cfg->name());
					return false;
				}

				// Finalise and store weighting matrix
				weightsMatrix.finalise();
				GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "PartialWeights", uniqueName_) = weightsMatrix.scatteringMatrix();

				// Calculate weighted partials
				PartialRSet& partialgr = GenericListHelper<PartialRSet>::realise(cfg->moduleData(), "UnweightedGR", uniqueName_);
				calculateWeighted(cfg, partialgr, weightsMatrix);
			}
		}

		// Setup partial set for the Sample, using the AtomTypeList we have just constructed.
		// We will use RDF range information from the first Configuration in the list
		Configuration* refConfig = targetConfigurations_.firstItem();
		PartialRSet& unweightedPartials = GenericListHelper<PartialRSet>::realise(sam->moduleData(), "UnweightedGR", uniqueName_);
		unweightedPartials.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), sam->niceName(), "unweighted", "rdf");
		atomTypes.finalise();
		atomTypes.print();

		// Loop over Configurations again, summing into the PartialRSet we have just set up
		// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
		configIterator.restart();
		while (Configuration* cfg = configIterator.iterate())
		{
			// Get weighting factor for this Configuration to contribute to the summed partials
			varName.sprintf("%s_Weight", cfg->name());
			double weight = 1.0;
			if (sam->moduleData().contains(varName, mixSource)) weight = GenericListHelper<double>::retrieve(sam->moduleData(), varName, mixSource, 1.0);
			totalWeight += weight;
			Messenger::print("Partials: Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);

			// Grab partials for Configuration
			PartialRSet& cfgPartials = GenericListHelper<PartialRSet>::retrieve(cfg->moduleData(), "UnweightedGR", uniqueName_);
			cfgPartials.save();

			// Add all partials from the Configuration into our Sample partials
			unweightedPartials.addPartials(cfgPartials, weight);
		}

		// Now must normalise Sample partials to the overall weight of the source configurations
		unweightedPartials.reweightPartials(1.0 / totalWeight);

		// Calculate weighted partials and total
// 		if (weightsType != Partials::NoWeighting) calculateWeighted(moduleSource, weights);
// 		PartialRSet& weightedPartials = GenericListHelper<PartialRSet>::realise(sam->moduleData(), CharString("WeightedGR", mixSource.get()), uniqueName_);
// 		weightedPartials.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), sam->niceName(), "weighted", "rdf");
		// TODO XXX

		// Save data?
		if (saveData)
		{
			// Only the pool master saves the data
			if (procPool.isMaster())
			{
				// Find PartialSet for this Configuration
				if (unweightedPartials.save()) procPool.proceed();
				else
				{
					procPool.stop();
					return false;
				}
			}
			else if (!procPool.decision()) return false;
		}
	}
	else
	{
		RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
		while (Configuration* cfg = configIterator.iterate())
		{
			// Retrieve control parameters from Configuration
			const bool saveData = GenericListHelper<bool>::retrieve(cfg->moduleData(), "Save", uniqueName(), options_.valueAsBool("Save"));

			// Print argument/parameter summary
			Messenger::print("Partials: Save data is %s.\n", DUQSys::onOff(saveData));

			calculateUnweighted(cfg, procPool);

			// Save data?
			if (saveData)
			{
				// Only the pool master saves the data
				if (procPool.isMaster())
				{
					// Find PartialSet for this Configuration
					PartialRSet& partials = GenericListHelper<PartialRSet>::retrieve(cfg->moduleData(), "UnweightedGR", uniqueName_);
					if (partials.save()) procPool.proceed();
					else
					{
						procPool.stop();
						return false;
					}
				}
				else if (!procPool.decision()) return false;
			}
		}
	}

	return true;
}

// Execute post-processing stage
bool Partials::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

/*
 * Members / Functions
 */

// Calculate partial RDFs with simple double-loop
bool Partials::calculateSimple(Configuration* cfg, PartialRSet& partialSet, ProcessPool& procPool)
{
	// Variables
	int n, m, nTypes, typeI, typeJ, i, j, nPoints;

	// Construct local arrays of atom type positions
	Messenger::printVerbose("Constructing local partial working arrays.\n");
	nTypes = partialSet.nTypes();
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
// 		printf("For types %i-%i, count = %i\n", typeI, typeI, count);
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
// 			printf("For types %i-%i count = %i, time = %s\n", typeI, typeJ, count, timer.timeString());
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

// Calculate unweighted partials for the specified Configuration
bool Partials::calculateUnweighted(Configuration* cfg, ProcessPool& procPool, int method)
{
	// Retrieve control parameters from Configuration
	const int smoothing = GenericListHelper<int>::retrieve(cfg->moduleData(), "Smoothing", uniqueName(), options_.valueAsInt("Smoothing"));

	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialRSet& partialgr = GenericListHelper<PartialRSet>::realise(cfg->moduleData(), "UnweightedGR", uniqueName_, &wasCreated);
	if (wasCreated) partialgr.setup(cfg, cfg->niceName(), "unweighted", "rdf");

	// Is the PartialSet already up-to-date?
	if (partialgr.index() == cfg->coordinateIndex())
	{
		Messenger::print("Partials: Partials are up-to-date for Configuration '%s'.\n", cfg->name());
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

	// if (method == 0) ...
	Timer timer;
	timer.start();
	procPool.resetAccumulatedTime();
	calculateSimple(cfg, partialgr, procPool);
	timer.stop();
	Messenger::print("Partials: Finished calculation of partials (%s elapsed, %s comms).\n", timer.timeString(), procPool.accumulatedTimeString());

	/*
	 * Calculate intramolecular partials
	 */

	double distance;
	int start, stride;
	const Box* box = cfg->box();

	// Set start/skip for parallel loop (pool solo)
	start = procPool.interleavedLoopStart(ProcessPool::OverPoolProcesses);
	stride = procPool.interleavedLoopStride(ProcessPool::OverPoolProcesses);

	timer.start();
	// Loop over molecules...
	Atom* i, *j, *k;
	for (int m=start; m<cfg->nMolecules(); m += stride)
	{
		Molecule* mol = cfg->molecule(m);

		// Bonds
		for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			i = mol->atom(b->indexI());
			j = mol->atom(b->indexJ());
			if (cfg->useMim(i->cell(), j->cell())) distance = box->minimumDistance(i, j);
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
			if (cfg->useMim(i->grain()->cell(), k->grain()->cell())) distance = box->minimumDistance(i, k);
			else distance = (i->r() - k->r()).magnitude();
			partialgr.boundHistogram(i->localTypeIndex(), k->localTypeIndex()).add(distance);
		}
	}
	timer.stop();
	Messenger::print("Partials: Finished calculation of intramolecular partials (%s elapsed, %s comms).\n", timer.timeString(), procPool.accumulatedTimeString());

	/*
	 * Sum hstogram data
	 */

	// Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix knows
	// that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.

	int typeI, typeJ;
	procPool.resetAccumulatedTime();
	timer.start();
	for (typeI=0; typeI<partialgr.nTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<partialgr.nTypes(); ++typeJ)
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
	Data2D boxNorm = cfg->boxNormalisation();
	partialgr.formPartials(box->volume(), boxNorm);

	// Smooth partials if requested
	if (smoothing > 0)
	{
		for (typeI=0; typeI<partialgr.nTypes(); ++typeI)
		{
			for (typeJ=typeI; typeJ<partialgr.nTypes(); ++typeJ)
			{
				partialgr.partial(typeI,typeJ).smooth(smoothing*2+1);
				partialgr.boundPartial(typeI,typeJ).smooth(smoothing*2+1);
				partialgr.unboundPartial(typeI,typeJ).smooth(smoothing*2+1);
			}
		}
	}

	// Sum total functions
	partialgr.formTotal();
	timer.stop();
	Messenger::print("Partials: Finished summation and normalisation of partial g(r) data (%s elapsed, %s comms).\n", timer.timeString(), procPool.accumulatedTimeString());

	/*
	 * Partials are now up-to-date
	 */

	partialgr.setIndex(cfg->coordinateIndex());
	++staticLogPoint_;

	return true;
}

// Calculate weighted partials (from existing partial) for the specified Configuration
bool Partials::calculateWeighted(Configuration* cfg, PartialRSet& unweightedPartials, WeightsMatrix& weightsMatrix)
{
	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialRSet& partialgr = GenericListHelper<PartialRSet>::realise(cfg->moduleData(), "WeightedGR", uniqueName_, &wasCreated);
	if (wasCreated) partialgr.setup(cfg, cfg->niceName(), "weighted", "rdf");

	int typeI, typeJ;
	for (typeI=0; typeI<partialgr.nTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<partialgr.nTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weightsMatrix.scatteringWeight(typeI, typeJ);
			partialgr.partial(typeI, typeJ).copyData(unweightedPartials.partial(typeI, typeJ));
			partialgr.partial(typeI, typeJ).arrayY() *= factor;
			partialgr.boundPartial(typeI, typeJ).copyData(unweightedPartials.boundPartial(typeI, typeJ));
			partialgr.boundPartial(typeI, typeJ).arrayY() *= factor;
			partialgr.unboundPartial(typeI, typeJ).copyData(unweightedPartials.unboundPartial(typeI, typeJ));
			partialgr.unboundPartial(typeI, typeJ).arrayY() *= factor;
// 			braggSQ.arrayY() *= factor;   TODO
		}
	}

	// Calculate total
	partialgr.formTotal();

	return true;
}

// 	// Calculate Bragg partials (if requested)
// 	if (braggOn)
// 	{
// 		procPool.resetAccumulatedTime();
// 		timer.start();
// 		if (!calculateBraggContributions(procPool)) return false;
// // 		if (!calculateBraggSQ(procPool)) return false;
// 		timer.stop();
// 		Messenger::print("--> Finished calculation of partial Bragg S(Q) (%s elapsed, %s comms).\n", timer.timeString(), procPool.accumulatedTimeString());
// 	}
// 
// 	// Generate final partial S(Q) combining pair correlations and Bragg partials
// 	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
// 	{
// 		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
// 		{
// 			// Grab references:
// 			// -- Full atom pair-pair structure factors (bound + unbound)
// 			Data2D& pairSQ = pairSQMatrix_.ref(typeI,typeJ);
// 			// -- Bragg partial structure factors
// 			Data2D& braggSQ = braggSQMatrix_.ref(typeI,typeJ);
// 			// -- Full partial structure factors (destination)
// 			Data2D& partialSQ = partialSQMatrix_.ref(typeI,typeJ);
// 
// 			// Copy atomic pair S(Q) information
// 			partialSQ = pairSQ;
// 
// 			// Combine Bragg(Q) data if it was calculated
// 			if (braggOn)
// 			{
// 				double xRange = 0.1, x;
// 				double xMin = braggMaximumQ_ - xRange - qDelta*0.5;
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

/*
 * Parallel Comms
 */

// Broadcast data associated to module
bool Partials::broadcastData(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Broadcast all data from this type of Module over the ProcessPool specified.
	 * This function should only ever be called on the root instance of the Module. Static data can be distributed here,
	 * as well as instance-local information (via the instances_ list).
	 */
#ifdef PARALLEL
	/*
	 * Static Data
	 */
	// Has the data been updated since the last broadcast? Use the 
	Messenger::printVerbose("Partials: This process thinks that the static/logpoints are %i and %i\n", staticBroadcastPoint_, staticLogPoint_);
	if (!procPool.allTrue(staticBroadcastPoint_ == staticLogPoint_))
	{
		Messenger::print("Partials: Broadcasting partialSets over processes...\n");
		// PartialSets - each process in the pool will loop over its own partialSets_ list and, for those in which it was the root
		// proces in the Configuration's process pool, it will broadcast the data. Other processes may not know about this Configuration's
		// PartialSet yet, so we should check for its existence and create it if it doesn't exist.
		CharString cfgName;
		for (int rootRank = 0; rootRank < procPool.nProcesses(); ++rootRank)
		{
			// Loop over partialSets_ - we must be careful only to broadcast those data for which the specified rootRank
			// is the root process in the associated Configuration's ProcessPool
			if (procPool.poolRank() == rootRank)
			{
				// The current rootRank will go through its list of partialSets_
				for (PartialRSet* ps = partialSets_.first(); ps != NULL; ps = ps->next)
				{
					Configuration* cfg = ps->targetConfiguration();
					if (!cfg->processPool().involvesMe()) continue;
					if (!cfg->processPool().isMaster()) continue;

					procPool.proceed(rootRank);
					// Broadcast name of Configuration that the partialSet targets
					cfgName = cfg->name();
					if (!procPool.broadcast(cfgName, rootRank)) return false;
					if (!ps->broadcast(procPool, rootRank)) return false;
				}

				procPool.stop(rootRank);
			}
			else
			{
				// Slaves wait to see if they will receive any data
				while (procPool.decision(rootRank))
				{
					// Receive name of Configuration from root process
					if (!procPool.broadcast(cfgName, rootRank)) return false;
					// Find named Configuration
					Configuration* cfg = duq.findConfiguration(cfgName);
					// Do we currently have a partialSet for this Configuration?
					PartialRSet* partialgr = partialSet(cfg);
					if (partialgr == NULL)
					{
						// No match, so create new
						partialgr = partialSets_.add();
						partialgr.setup(cfg, "unweighted", "rdf");
					}
					if (!partialgr.broadcast(procPool, rootRank)) return false;
				}
			}
		}

		// Update logPoint_
		staticBroadcastPoint_ = staticLogPoint_;
	}
#endif
	return true;
}
