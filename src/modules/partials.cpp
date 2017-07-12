/*
	*** Partials Module
	*** src/modules/partials.cpp
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

#include "main/duq.h"
#include "modules/partials.h"
#include "classes/box.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "classes/weightsmatrix.h"
#include "classes/partialqset.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include "base/timer.h"

// Static Members
List<Module> PartialsModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
PartialsModule::PartialsModule() : Module()
{
	// Add to instances list and set unique name for this instance
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems());
	instances_.own(this);

	// Setup variables / control parameters
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	frequency_ = 5;
	options_.add("Bragg", bool(false), "Enable calculation of Bragg scattering", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("BraggQDepBroadening", 0.0063, "FWHM of Gaussian for Q-dependent Bragg broadening function", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("BraggQIndepBroadening", 0.0, "FWHM of Gaussian for Q-independent Bragg broadening function", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("NormaliseToAvSq", bool(false), "Normalise calculated F(Q) to < b >**2", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("NormaliseToSqAv", bool(false), "Normalise calculated F(Q) to < b**2 >", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QDelta", 0.05, "Step size in Q for S(Q) calculation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QDepBroadening", 0.0, "FWHM of Gaussian for Q-dependent instrument broadening function when calculating S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QIndepBroadening", 0.0, "FWHM of Gaussian for Q-independent instrument broadening function when calculating S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QMax", -1.0, "Maximum Q for calculated S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Save", bool(false), "Whether to save partials to disk after calculation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Smoothing", 0, "Specifies the degree of smoothing 'n' to apply to calculated RDFs, where 2n+1 controls the length in the applied Spline smooth", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("StructureFactor", bool(false), "Determined whether S(Q) are calculated from the g(r)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Test", bool(false), "Test calculated total G(r) and/or F(Q) against supplied reference data", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestThreshold", 0.1, "Test threshold (RMSE) above which test fails", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Weights", "None", "Weighting scheme to use for calculated partials (None,Neutron)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
}

// Destructor
PartialsModule::~PartialsModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& PartialsModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* PartialsModule::createInstance()
{
	return new PartialsModule;
}

/*
 * Definition
 */

// Return name of module
const char* PartialsModule::name()
{
	return "Partials";
}

// Return brief description of module
const char* PartialsModule::brief()
{
	return "Calculate partial and total g(r) and S(Q)";
}

// Return instance type for module
Module::InstanceType PartialsModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool PartialsModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool PartialsModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool PartialsModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* PartialsModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool PartialsModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int PartialsModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	if (DUQSys::sameString(parser.argc(0), "Isotopologue"))
	{
		// Essentially a shortcut for setting a variable in a target Configuration / Sample
		// Find target Configuration
		Configuration* targetCfg = duq->findConfiguration(parser.argc(1));
		if (!targetCfg)
		{
			Messenger::error("Error defining Isotopologue - no Configuration named '%s' exists.\n", parser.argc(1));
			return false;
		}

		// Raise an error if this Configuration is not targetted by the Module
		if (!isTargetConfiguration(targetCfg)) 
		{
			Messenger::error("Configuration '%s' is not targetted by the Module '%s'.\n", targetCfg->name(), name());
			return false;
		}

		// Find specified Species - must be present in the target Configuration
		Species* sp = duq->findSpecies(parser.argc(2));
		if (!sp)
		{
			Messenger::error("Error defining Isotopologue - no Species named '%s' exists.\n", parser.argc(2));
			return false;
		}

		if (!targetCfg->usedSpecies().contains(sp))
		{
			Messenger::error("Error defining Isotopologue - Species '%s' is not present in Configuration '%s'.\n", sp->name(), targetCfg->name());
			return false;
		}

		// Finally, locate isotopologue definition for species
		Isotopologue* tope = sp->findIsotopologue(parser.argc(3));
		if (!tope)
		{
			Messenger::error("Error defining Isotopologue - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(3), sp->name());
			return false;
		}

		// Ready - add a suitable variable to the Configuration
		CharString varName("Isotopologue/%s/%s", sp->name(), tope->name());
		GenericListHelper<double>::add(targetList, varName, uniqueName()) = parser.argd(4);
	}
	else if (DUQSys::sameString(parser.argc(0), "TestReferenceSQ"))
	{
		Messenger::print("Reading test reference structure factor data...\n");

		// Realise an XYData to store the reference data in
		XYData& data = GenericListHelper<XYData>::realise(targetList, CharString("ReferenceSQ-%s", parser.argc(2)), uniqueName(), GenericItem::NoOutputFlag);

		// Fourth and fifth arguments are x and y columns respectively (defaulting to 0,1 if not given)
		int xcol = parser.hasArg(3) ? parser.argi(3) : 0;
		int ycol = parser.hasArg(4) ? parser.argi(4) : 1;

		LineParser fileParser(&duq->worldPool());
		if ((!fileParser.openInput(parser.argc(1))) || (!data.load(fileParser, xcol, ycol))) return false;
	}
	else if (DUQSys::sameString(parser.argc(0), "TestReferenceGR"))
	{
		Messenger::print("Reading test reference radial distribution function data...\n");

		// Realise an XYData to store the reference data in
		XYData& data = GenericListHelper<XYData>::realise(targetList, CharString("ReferenceGR-%s", parser.argc(2)), uniqueName(), GenericItem::NoOutputFlag);

		// Fourth and fifth arguments are x and y columns respectively (defaulting to 0,1 if not given)
		int xcol = parser.hasArg(3) ? parser.argi(3) : 0;
		int ycol = parser.hasArg(4) ? parser.argi(4) : 1;

		LineParser fileParser(&duq->worldPool());
		if ((!fileParser.openInput(parser.argc(1))) || (!data.load(fileParser, xcol, ycol))) return false;
	}
	else return -1;

	return true;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int PartialsModule::nTargetableConfigurations()
{
	return (configurationLocal_ ? 1 : -1);
}

/*
 * Method
 */

// Perform setup tasks for module
bool PartialsModule::setup(ProcessPool& procPool)
{
	return false;
}

// Execute pre-processing stage
bool PartialsModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool PartialsModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate weighted or unweighted partials and total g(r)/G(r) or S(Q)/F(Q) for the specified Sample or Configurations
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */
	CharString varName;

	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();
	const bool braggOn = GenericListHelper<bool>::retrieve(moduleData, "Bragg", uniqueName(), options_.valueAsBool("Bragg"));
	const double braggQDepBroadening = GenericListHelper<double>::retrieve(moduleData, "BraggQDepBroadening", uniqueName(), options_.valueAsDouble("BraggQDepBroadening"));
	const double braggQIndepBroadening = GenericListHelper<double>::retrieve(moduleData, "BraggQIndepBroadening", uniqueName(), options_.valueAsDouble("BraggQIndepBroadening"));
	const double qDepBroadening = GenericListHelper<double>::retrieve(moduleData, "QDepBroadening", uniqueName(), options_.valueAsDouble("QDepBroadening"));
	const double qIndepBroadening = GenericListHelper<double>::retrieve(moduleData, "QIndepBroadening", uniqueName(), options_.valueAsDouble("QIndepBroadening"));
	const bool normaliseToAvSq = GenericListHelper<bool>::retrieve(moduleData, "NormaliseToAvSq", uniqueName(), options_.valueAsBool("NormaliseToAvSq"));
	const bool normaliseToSqAv = GenericListHelper<bool>::retrieve(moduleData, "NormaliseToSqAv", uniqueName(), options_.valueAsBool("NormaliseToSqAv"));
	const double qDelta = GenericListHelper<double>::retrieve(moduleData, "QDelta", uniqueName(), options_.valueAsDouble("QDelta"));
	double qMax = GenericListHelper<double>::retrieve(moduleData, "QMax", uniqueName(), options_.valueAsDouble("QMax"));
	if (qMax < 0.0) qMax = 30.0;
	const bool saveData = GenericListHelper<bool>::retrieve(moduleData, "Save", uniqueName(), options_.valueAsBool("Save"));
	const int smoothing = GenericListHelper<int>::retrieve(moduleData, "Smoothing", uniqueName(), options_.valueAsInt("Smoothing"));
	const bool sqCalculation = GenericListHelper<bool>::retrieve(moduleData, "StructureFactor", uniqueName(), options_.valueAsBool("StructureFactor"));
	const bool testMode = GenericListHelper<bool>::retrieve(moduleData, "Test", uniqueName(), options_.valueAsBool("Test"));
	const double testThreshold = GenericListHelper<bool>::retrieve(moduleData, "TestThreshold", uniqueName(), options_.valueAsBool("TestThreshold"));
	CharString weightsString = GenericListHelper<CharString>::retrieve(moduleData, "Weights", uniqueName_, options_.valueAsString("Weights"));
	PartialsModule::WeightingType weightsType = PartialsModule::NoWeighting;
	if (DUQSys::sameString(weightsString, "Neutron")) weightsType = PartialsModule::NeutronWeighting;
	else if (!DUQSys::sameString(weightsString, "None"))
	{
		Messenger::error("Partials: Invalid weighting scheme '%s' found.\n", weightsString.get());
		return false;
	}

	// Print argument/parameter summary
	Messenger::print("Partials: Calculating S(Q)/F(Q) out to %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMax, qDelta);
	Messenger::print("Partials: Q-dependent FWHM broadening to use is %f, Q-independent FWHM broadening to use is %f.\n", qDepBroadening, qIndepBroadening);
	Messenger::print("Partials: Calculation of Bragg features is %s.\n", DUQSys::onOff(braggOn));
	if (braggOn) Messenger::print("Partials: Q-dependent FWHM Bragg broadening to use is %f, Q-independent FWHM Bragg broadening to use is %f.\n", braggQDepBroadening, braggQIndepBroadening);
	Messenger::print("Partials: Save data is %s.\n", DUQSys::onOff(saveData));
	Messenger::print("Partials: Structure factor calculation is %s.\n", DUQSys::onOff(sqCalculation));
	if (sqCalculation)
	{
		Messenger::print("Partials: Structure factor QMax = %f, QDelta = %f.\n", qMax, qDelta);
		Messenger::print("Partials: Bragg calculationg is %s.\n", DUQSys::onOff(saveData));
		if (braggOn) Messenger::print("Partials: Bragg Q-dependent FWHM broadening to use is %f, Q-independent FWHM broadening to use is %f.\n", braggQDepBroadening, braggQIndepBroadening);
	}

	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Calculate unweighted partials for this Configuration
		calculateUnweightedGR(procPool, cfg, smoothing);
		PartialRSet& unweightedgr = GenericListHelper<PartialRSet>::retrieve(cfg->moduleData(), "UnweightedGR", uniqueName_);
		if (saveData && (!configurationLocal_) && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;

		// Calculate S(Q) if requested
		if (sqCalculation)
		{
			// Realise a PartialSQ set
			PartialQSet& unweightedsq = GenericListHelper<PartialQSet>::realise(cfg->moduleData(), "UnweightedSQ", uniqueName_, GenericItem::NoOutputFlag);
			unweightedsq.setup(unweightedgr.atomTypes(), cfg->niceName(), "unweighted", "sq");
			calculateUnweightedSQ(procPool, unweightedgr, unweightedsq, qMax, qDelta, cfg->atomicDensity(), duq.windowFunction(), qDepBroadening, qIndepBroadening, braggOn, braggQDepBroadening, braggQIndepBroadening);

			if (saveData && (!configurationLocal_) && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;
		}

		// Calculate weighted partials here if requested
		if (weightsType != PartialsModule::NoWeighting)
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
					if (moduleData.contains(varName, uniqueName_))
					{
						// This isotopologue is defined as being used, so add its (in the isotopic proportions defined in the Isotopologue) to the weightsMatrix.
						weightsMatrix.addIsotopologue(sp, speciesPopulation, availableIso, GenericListHelper<double>::retrieve(moduleData, varName, uniqueName_));
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
			GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "PartialWeights", uniqueName_, GenericItem::NoOutputFlag) = weightsMatrix.scatteringMatrix();

			// Calculate weighted partials
			PartialRSet& weightedgr = GenericListHelper<PartialRSet>::realise(cfg->moduleData(), "WeightedGR", uniqueName_, GenericItem::NoOutputFlag);
			PartialQSet& weightedsq = GenericListHelper<PartialQSet>::realise(cfg->moduleData(), "WeightedSQ", uniqueName_, GenericItem::NoOutputFlag);
			weightedgr.setup(cfg, cfg->niceName(), "weighted", "rdf");
			calculateWeightedGR(unweightedgr, weightedgr, weightsMatrix);
			if (sqCalculation)
			{
				PartialQSet& unweightedsq = GenericListHelper<PartialQSet>::retrieve(cfg->moduleData(), "UnweightedSQ", uniqueName_);
				weightedsq.setup(weightedgr.atomTypes(), cfg->niceName(), "weighted", "sq");
				calculateWeightedSQ(unweightedsq, weightedsq, weightsMatrix);
			}

			if (saveData && (!configurationLocal_))
			{
				if (!MPIRunMaster(procPool, weightedgr.save())) return false;
				if (sqCalculation && (!MPIRunMaster(procPool, weightedsq.save()))) return false;
			}

		}

		// Test mode
		if (testMode)
		{
		}
	}

	// If we are a main processing task, construct the weighted sum of Configuration
	if (!configurationLocal_)
	{
		// Assemble partials from all target Configurations specified, weighting them accordingly
		CharString varName;
		AtomTypeList atomTypes;

		// Loop over Configurations, creating a list of unique AtomTypes encountered over all.
		RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
		while (Configuration* cfg = configIterator.iterate())
		{
			// Add any missing atom types into our local list
			atomTypes.add(cfg->usedAtomTypesList());
		}
		atomTypes.finalise();
		atomTypes.print();

		// Setup partial set using the AtomTypeList we have just constructed.
		// We will use RDF range information from the first Configuration in the list
		Configuration* refConfig = targetConfigurations_.firstItem();
		PartialRSet& unweightedgr = GenericListHelper<PartialRSet>::realise(duq.processingModuleData(), "UnweightedGR", uniqueName_, GenericItem::NoOutputFlag);
		unweightedgr.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "unweighted", "rdf");

		// Loop over Configurations again, summing into the PartialRSet we have just set up
		// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
		double totalWeight = 0.0, density = 0.0, volume = 0.0;
		configIterator.restart();
		while (Configuration* cfg = configIterator.iterate())
		{
			// Get weighting factor for this Configuration to contribute to the summed partials
			double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);
			totalWeight += weight;
			Messenger::print("Partials: Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);

			// Grab partials for Configuration and add into our set
			PartialRSet& cfgPartials = GenericListHelper<PartialRSet>::retrieve(cfg->moduleData(), "UnweightedGR", uniqueName_);
			unweightedgr.addPartials(cfgPartials, weight);

			// Sum density weighted by volume, and total volume (both of which are multiplied by the overall weight)
			density += cfg->atomicDensity()*cfg->box()->volume()*weight;
			volume += cfg->box()->volume()*weight;
		}
		density /= volume;

		// Now must normalise our partials to the overall weight of the source configurations
		unweightedgr.reweightPartials(1.0 / totalWeight);
		if (saveData) if (!MPIRunMaster(procPool, unweightedgr.save())) return false;

		// Calculate S(Q) if requested
		if (sqCalculation)
		{
			// Realise a PartialSQ set
			PartialQSet& unweightedsq = GenericListHelper<PartialQSet>::realise(moduleData, "UnweightedSQ", uniqueName_, GenericItem::NoOutputFlag);
			unweightedsq.setup(unweightedgr.atomTypes(), uniqueName(), "unweighted", "sq");

			// Sum in weighted S(Q) partials
			configIterator.restart();
			while (Configuration* cfg = configIterator.iterate())
			{
				// Get unweighted SQ for this Configuration and their overall weight
				PartialQSet& cfgunweightedsq = GenericListHelper<PartialQSet>::retrieve(cfg->moduleData(), "UnweightedSQ", uniqueName_);
				double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

				// Add into our set
				unweightedsq.addPartials(cfgunweightedsq, weight);
			}

			// Now must normalise our partials to the overall weight of the source configurations
			unweightedsq.reweightPartials(1.0 / totalWeight);
			if (saveData && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;
		}

		// Calculate weighted Sample partials and total if requested
		if (weightsType != PartialsModule::NoWeighting)
		{
			PartialRSet& weightedgr = GenericListHelper<PartialRSet>::realise(moduleData, "WeightedGR", uniqueName_, GenericItem::NoOutputFlag);
			weightedgr.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "weighted", "rdf");
			weightedgr.reset();

			// Loop over Configurations, adding in their weighted partial sets as we go
			configIterator.restart();
			while (Configuration* cfg = configIterator.iterate())
			{
				// Grab weighted partials for this Configuration and their associated weight
				PartialRSet& cfgweightedgr = GenericListHelper<PartialRSet>::retrieve(cfg->moduleData(), "WeightedGR", uniqueName_);
				double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

				weightedgr.addPartials(cfgweightedgr, weight);
			}

			// Re-weight partial set according to totalWeight (calculated earlier)
			weightedgr.reweightPartials(1.0 / totalWeight);
			if (saveData && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;

			// Calculate S(Q) if requested
			if (sqCalculation)
			{
				// Realise a PartialSQ set
				PartialQSet& weightedsq = GenericListHelper<PartialQSet>::realise(moduleData, "WeightedSQ", uniqueName_, GenericItem::NoOutputFlag);
				weightedsq.setup(unweightedgr.atomTypes(), uniqueName(), "weighted", "sq");

				// Sum in weighted S(Q) partials
				configIterator.restart();
				while (Configuration* cfg = configIterator.iterate())
				{
					// Get unweighted SQ for this Configuration and their overall weight
					PartialQSet& cfgweightedsq = GenericListHelper<PartialQSet>::retrieve(cfg->moduleData(), "WeightedSQ", uniqueName_);
					double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

					// Add into our set
					weightedsq.addPartials(cfgweightedsq, weight);
				}

				// Now must normalise our partials to the overall weight of the source configurations
				weightedsq.reweightPartials(1.0 / totalWeight);
				if (saveData && (!MPIRunMaster(procPool, weightedsq.save()))) return false;
			}
		}
	}

	return true;
}

// Execute post-processing stage
bool PartialsModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

/*
 * Members / Functions
 */

// Calculate partial RDFs with simple double-loop
bool PartialsModule::calculateSimple(Configuration* cfg, PartialRSet& partialSet, ProcessPool& procPool)
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

// Calculate unweighted partials for the specified Configuration
bool PartialsModule::calculateUnweightedGR(ProcessPool& procPool, Configuration* cfg, int smoothing)
{
	// Does a PartialSet already exist for this Configuration?
	bool wasCreated;
	PartialRSet& partialgr = GenericListHelper<PartialRSet>::realise(cfg->moduleData(), "UnweightedGR", uniqueName_, GenericItem::NoOutputFlag, &wasCreated);
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
	Messenger::print("Partials: Finished calculation of partials (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

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
			if (cfg->useMim(i->cell(), k->cell())) distance = box->minimumDistance(i, k);
			else distance = (i->r() - k->r()).magnitude();
			partialgr.boundHistogram(i->localTypeIndex(), k->localTypeIndex()).add(distance);
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
	partialgr.formTotal();
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
bool PartialsModule::calculateWeightedGR(PartialRSet& unweightedgr, PartialRSet& weightedgr, WeightsMatrix& weightsMatrix)
{
	int typeI, typeJ;
	for (typeI=0; typeI<weightedgr.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<weightedgr.nAtomTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weightsMatrix.scatteringWeight(typeI, typeJ);
			weightedgr.partial(typeI, typeJ).copyData(unweightedgr.partial(typeI, typeJ));
			weightedgr.partial(typeI, typeJ).arrayY() *= factor;
			weightedgr.boundPartial(typeI, typeJ).copyData(unweightedgr.boundPartial(typeI, typeJ));
			weightedgr.boundPartial(typeI, typeJ).arrayY() *= factor;
			weightedgr.unboundPartial(typeI, typeJ).copyData(unweightedgr.unboundPartial(typeI, typeJ));
			weightedgr.unboundPartial(typeI, typeJ).arrayY() *= factor;
// 			braggSQ.arrayY() *= factor;   TODO
		}
	}

	// Calculate total
	weightedgr.formTotal();

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
bool PartialsModule::calculateUnweightedSQ(ProcessPool& procPool, PartialRSet& sourcegr, PartialQSet& destsq, double qMax, double qDelta, double rho, XYData::WindowFunction wf, double qDepBroadening, double qIndepBroadening, bool includeBragg, double qDepBraggBroadening, double qIndepBraggBroad)
{
	// Copy partial g(r) into our new S(Q) matrices
	Messenger::printVerbose("  --> Copying partial g(r) into S(Q) matrices...\n");
	int nTypes = sourcegr.nAtomTypes();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			// Copy g(r) data into our arrays
			// Subtract 1.0 from the full and unbound partials so as to give (g(r)-1)
			// Don't subtract 1.0 from the bound partials, since they do not tend to 1.0 at longer r??
			destsq.partial(n,m).copyData(sourcegr.partial(n,m));
			destsq.partial(n,m).arrayY() -= 1.0;
			destsq.boundPartial(n,m).copyData(sourcegr.boundPartial(n,m));
// 			destsq.boundPartial(n,m).arrayY() -= 1.0;
			destsq.unboundPartial(n,m).copyData(sourcegr.unboundPartial(n,m));
			destsq.unboundPartial(n,m).arrayY() -= 1.0;
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
			if (!destsq.partial(n,m).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;
			if (!destsq.boundPartial(n,m).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;
			if (!destsq.unboundPartial(n,m).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, wf)) return false;
		}
	}

	// Sum into total
	destsq.formTotal();

	timer.stop();
	Messenger::print("Partials: Finished Fourier transform and summation of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * S(Q) are now up-to-date
	 */

	destsq.setIndex(sourcegr.index());

	return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q)
bool PartialsModule::calculateWeightedSQ(PartialQSet& unweightedsq, PartialQSet& weightedsq, WeightsMatrix& weightsMatrix)
{
	int typeI, typeJ;
	for (typeI=0; typeI<unweightedsq.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<unweightedsq.nAtomTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weightsMatrix.scatteringWeight(typeI, typeJ);
			weightedsq.partial(typeI, typeJ).copyData(unweightedsq.partial(typeI, typeJ));
			weightedsq.partial(typeI, typeJ).arrayY() *= factor;
			weightedsq.boundPartial(typeI, typeJ).copyData(unweightedsq.boundPartial(typeI, typeJ));
			weightedsq.boundPartial(typeI, typeJ).arrayY() *= factor;
			weightedsq.unboundPartial(typeI, typeJ).copyData(unweightedsq.unboundPartial(typeI, typeJ));
			weightedsq.unboundPartial(typeI, typeJ).arrayY() *= factor;
// 			braggSQ.arrayY() *= factor;   TODO
		}
	}

	// Calculate total
	weightedsq.formTotal();

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
// 		Messenger::print("--> Finished calculation of partial Bragg S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());
// 	}
// 
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
bool PartialsModule::broadcastData(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Broadcast all data from this type of Module over the ProcessPool specified.
	 * This function should only ever be called on the root instance of the Module. Static data can be distributed here,
	 * as well as instance-local information (via the instances_ list).
	 */
#ifdef PARALLEL
#endif
	return true;
}
