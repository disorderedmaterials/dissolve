/*
	*** Structure Factor Module
	*** src/modules/fq.cpp
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
#include "modules/fq.h"
#include "modules/partials.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include "base/timer.h"
#include "math/matrix4.h"

// Static Members
List<Module> StructureFactor::instances_;
List<PartialQSet> StructureFactor::partialSets_;

/*
 * Constructor / Destructor
 */

// Constructor
StructureFactor::StructureFactor() : Module()
{
	// Setup variables / control parameters
	addVariable("Bragg", "off", "Enable calculation of Bragg scattering");
	addVariable("BraggQDepBroadening", 0.0063, "FWHM of Gaussian for Q-dependent Bragg broadening function");
	addVariable("BraggQIndepBroadening", 0.0, "FWHM of Gaussian for Q-dependent Bragg broadening function");
	addVariable("NormaliseToAvSq", false, "Normalise calculated F(Q) to < b >**2");
	addVariable("NormaliseToSqAv", false, "Normalise calculated F(Q) to < b**2 >");
	addVariable("QDelta", 0.05, "Step size in Q");
	addVariable("QDepBroadening", 0.0, "FWHM of Gaussian for Q-dependent instrument broadening function");
	addVariable("QIndepBroadening", 0.0, "FWHM of Gaussian for Q-independent instrument broadening function");
	addVariable("QMax", -1.0, "Maximum Q in calculated F(Q)");
	addVariable("Save", false, "Whether to save partials to disk after calculation");
}

// Destructor
StructureFactor::~StructureFactor()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& StructureFactor::instances()
{
	return instances_;
}

// Create instance of this module
Module* StructureFactor::createInstance()
{
	return new StructureFactor;
}

/*
 * Definition
 */

// Return name of module
const char* StructureFactor::name()
{
	return "StructureFactor";
}

// Return brief description of module
const char* StructureFactor::brief()
{
	return "Calculate total and partial structure factors";
}

// Return instance type for module
Module::InstanceType StructureFactor::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool StructureFactor::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool StructureFactor::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool StructureFactor::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* StructureFactor::dependentModules()
{
	return "Partials";
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int StructureFactor::nTargetableConfigurations()
{
	return 1;
}

// Return the maximum number of Samples the Module can target (or -1 for any number)
int StructureFactor::nTargetableSamples()
{
	return 0;
}

/*
 * Method
 */

// Perform setup tasks for module
bool StructureFactor::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool StructureFactor::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool StructureFactor::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate weighted or unweighted structure factors for the Sample or Configuration
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */

	// If there is a Sample target, then we calculate the weighted structure factors for it (using the supplied Configurations)
	// Otherwise take the Configuration targets and calculate unweighted structure factors for them.

	if (targetSamples_.nItems() > 0)
	{
		// TODO Assemble partials from all Configurations specified, weighting them accordingly
// 		if ((nSampleTargets() == 1) && (targetSamples_.first()->item->hasReferenceData())) qMax = targetSamples_.first()->item->referenceData().xMax();
	}
	else
	{
		RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
		while (Configuration* cfg = configIterator.iterate())
		{
			// Retrieve control parameters from Configuration
			const double qDelta = variableAsDouble(cfg, "QDelta");
			const double qMax = variableAsDouble(cfg, "QMax") < 0.0 ? 30.0 : variableAsDouble(cfg, "QMax");
			const bool braggOn = variableAsBool(cfg, "Bragg");
			const double braggQDepBroadening = variableAsDouble(cfg, "BraggQDepBroadening");
			const double braggQIndepBroadening = variableAsDouble(cfg, "BraggQIndepBroadening");
			const double qDepBroadening = variableAsDouble(cfg, "QDepBroadening");
			const double qIndepBroadening = variableAsDouble(cfg, "QIndepBroadening");
			const bool normaliseToAvSq = variableAsBool(cfg, "NormaliseToAvSq");
			const bool normaliseToSqAv = variableAsBool(cfg, "NormaliseToSqAv");
			const bool saveData = variableAsBool(cfg, "Save");

			// Print argument/parameter summary
			Messenger::print("StructureFactor: Calculating S(Q)/F(Q) out to %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMax, qDelta);
			Messenger::print("StructureFactor: Q-dependent FWHM broadening to use is %f, Q-independent FWHM broadening to use is %f.\n", qDepBroadening, qIndepBroadening);
			Messenger::print("StructureFactor: Calculation of Bragg features is %s.\n", DUQSys::onOff(braggOn));
			if (braggOn) Messenger::print("StructureFactor: Q-dependent FWHM Bragg broadening to use is %f, Q-independent FWHM Bragg broadening to use is %f.\n", braggQDepBroadening, braggQIndepBroadening);

			// Calculate structure factors for this Configuration
			calculateUnweighted(cfg, duq.windowFunction(), procPool);

			// Save data?
			if (saveData)
			{
				// Only the pool master saves the data
				if (procPool.isMaster())
				{
					// Find PartialSet for this Configuration
					PartialQSet* partials = partialSet(cfg);
					if (partials->save()) procPool.proceed();
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
}

// Return PartialSet for specified Configuration (if it exists)
PartialQSet* StructureFactor::partialSet(Configuration* cfg)
{
	// Search existing list
	for (PartialQSet* ps = partialSets_.first(); ps != NULL; ps = ps->next) if (ps->targetConfiguration() == cfg) return ps;

	return NULL;
}

// Calculate unweighted S(Q) for the specified Configuration
bool StructureFactor::calculateUnweighted(Configuration* cfg, Data2D::WindowFunction windowFunction, ProcessPool& procPool)
{
	// Retrieve control parameters from Configuration
	const double qDelta = variableAsDouble(cfg, "QDelta");
	const double qMax = variableAsDouble(cfg, "QMax") < 0.0 ? 30.0 : variableAsDouble(cfg, "QMax");
	const bool braggOn = variableAsBool(cfg, "Bragg");
	const double braggQDepBroadening = variableAsDouble(cfg, "BraggQDepBroadening");
	const double braggQIndepBroadening = variableAsDouble(cfg, "BraggQIndepBroadening");
	const double qDepBroadening = variableAsDouble(cfg, "QDepBroadening");
	const double qIndepBroadening = variableAsDouble(cfg, "QIndepBroadening");
	const bool normaliseToAvSq = variableAsBool(cfg, "NormaliseToAvSq");
	const bool normaliseToSqAv = variableAsBool(cfg, "NormaliseToSqAv");

	// Get the associated Partials module pointer so we can utilise it
	Partials* partialsModule = (Partials*) dependentModule("Partials");
	if (!partialsModule)
	{
		Messenger::error("No Partials module was associated to this instance of StructureFactor.\n");
		return false;
	}

	// Ensure that partials are up-to-date for the Configuration, and grab the PartialSet
	partialsModule->calculateUnweighted(cfg, procPool);
	PartialRSet* partialRDFs = Partials::partialSet(cfg);

	// Create / grab PartialSet for structure factors
	PartialQSet* partialSQ = StructureFactor::partialSet(cfg);
	if (partialSQ == NULL)
	{
		// No match, so create new
		partialSQ = partialSets_.add();
		partialSQ->setup(cfg, "unweighted", "sq");
	}

	// Is the PartialSet already up-to-date?
	if (partialSQ->upToDate())
	{
		Messenger::print("No need to calculate S(Q) for Configuration '%s' - nothing has changed since the last calculation.\n", cfg->name());
		return true;
	}

	Messenger::print("Calculating partial S(Q) for Configuration '%s'...\n", cfg->name());

	/*
	 * Reset any existing data
	 */

	partialSQ->reset();

	/*
	 * Copy g(r) data into our S(Q) arrays
	 */

	// Copy partial data into our own PartialSet
	int typeI, typeJ;
	procPool.resetAccumulatedTime();
	for (typeI=0; typeI<partialSQ->nTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<partialSQ->nTypes(); ++typeJ)
		{
			// All data except that for bound interactions must have 1.0 subtracted in order to ??????
			partialSQ->partial(typeI,typeJ).copyData(partialRDFs->partial(typeI,typeJ).normalisedData());
			partialSQ->partial(typeI,typeJ).arrayY() -= 1.0;
			partialSQ->boundPartial(typeI,typeJ).copyData(partialRDFs->boundPartial(typeI,typeJ).normalisedData());
// 			partialSQ->boundPartial(typeI,typeJ).arrayY() -= 1.0;
			partialSQ->unboundPartial(typeI,typeJ).copyData(partialRDFs->unboundPartial(typeI,typeJ).normalisedData());
			partialSQ->unboundPartial(typeI,typeJ).arrayY() -= 1.0;
		}
	}

	/*
	 * Perform FT of partial g(r) into S(Q)
	 */
	// TODO Parallelise this
	procPool.resetAccumulatedTime();
	Timer timer;
	timer.start();
	double rho = cfg->atomicDensity();
	for (typeI=0; typeI<partialSQ->nTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<partialSQ->nTypes(); ++typeJ)
		{
			if (!partialSQ->partial(typeI,typeJ).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, windowFunction)) return false;
			if (!partialSQ->boundPartial(typeI,typeJ).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, windowFunction)) return false;
			if (!partialSQ->unboundPartial(typeI,typeJ).transformBroadenedRDF(rho, qDelta, qMax, qDepBroadening, qIndepBroadening, windowFunction)) return false;
		}
	}

	// Sum into total
	partialSQ->formTotal();

	timer.stop();
	Messenger::print("--> Finished Fourier transform and summation of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.timeString(), procPool.accumulatedTimeString());

	/*
	 * Partials are now up-to-date
	 */

	partialSQ->setUpToDate();

	return true;
}

// // Calculate weighted pair correlations from supplied unweighted data
// bool Sample::calculatePairCorrelations(Array2D<Histogram>& masterRDFs, Array2D<Data2D>& masterPairSQ, Array2D<Data2D>& masterBraggSQ, Array2D<Data2D>& masterPartialSQ)
// {
// 	AtomTypeData* at1 = typeIndex_.first(), *at2;
// 	double factor, sumFactor = 0.0, braggMax;
// 	int typeI, typeJ, masterI, masterJ;
// 	
// 	Messenger::print("--> Calculating RDFs/S(Q)/F(Q) for Sample '%s'...\n", name_.get());
// 	
// 	// Setup totalGR_ and totalFQ_ arrays...
// 	totalGR_.arrayY() = 0.0;
// 	if (totalFQ_.nPoints() == 0)
// 	{
// 		totalFQ_.arrayX() = masterPairSQ.ref(0,0).arrayX();
// 		totalFQ_.arrayY() = masterPairSQ.ref(0,0).arrayY();
// 	}
// 	totalFQ_.arrayY() = 0.0;
// 	
// 	// Loop over atom types and sum data
// 	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI, at1 = at1->next)
// 	{
// 		// Grab master index of AtomType at1
// 		masterI = at1->masterIndex();
// #ifdef CHECKS
// 		if (masterI == -1)
// 		{
// 			Messenger::print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at1->name());
// 			return false;
// 		}
// #endif
// 
// 		at2 = at1;
// 		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ, at2 = at2->next)
// 		{
// 			// Grab master index of AtomType at2 and references to partial and bragg S(Q) matrix items
// 			masterJ = at2->masterIndex();
// 			Data2D& pairSQ = pairSQMatrix_.ref(typeI, typeJ);
// 			Data2D& braggSQ = braggSQMatrix_.ref(typeI, typeJ);
// 			Data2D& partialSQ = partialSQMatrix().ref(typeI, typeJ);
// #ifdef CHECKS
// 			if (masterJ == -1)
// 			{
// 				Messenger::print("INTERNAL_ERROR - Master index for AtomType '%s' has not been set.\n", at2->name());
// 				return false;
// 			}
// #endif
// 			// Copy unweighted S(Q) and Bragg data
// 			pairSQ  = masterPairSQ.ref(masterI, masterJ);
// 			braggSQ = masterBraggSQ.ref(masterI, masterJ);
// 			partialSQ = masterPartialSQ.ref(masterI, masterJ);
// 
// 			// Weight S(Q), Bragg S(Q) and full partial S(Q)
// 			factor = weightsMatrix_.ref(typeI,typeJ);
// 			pairSQ.arrayY() *= factor;
// 			braggSQ.arrayY() *= factor;
// 			partialSQ.arrayY() *= factor;
// 
// 			// Sum G(r) into totals
// 			totalGR_.addY(masterRDFs.ref(masterI,masterJ).normalisedData().arrayY(), factor);
// 			sumFactor += factor;
// 
// 			// Sum partialSQ into totalFQ_, weighting dissimilar partials accordingly
// 			totalFQ_.addY(partialSQ.arrayY(), typeI == typeJ ? 1.0 : 2.0);
// 		}
// 	}
// 
// 	// Finally, interpolate F(Q)
// 	totalFQ_.interpolate();
// 
// 	// Calculate difference F(Q)
// 	differenceData_.clear();
// 	if (hasReferenceData_)
// 	{
// 		double Q;
// 		for (int n=0; n<referenceData_.nPoints(); ++n)
// 		{
// 			Q = referenceData_.x(n);
// 			if (Q > totalFQ_.arrayX().last()) break;
// 			differenceData_.addPoint(Q, totalFQ_.interpolated(Q) - referenceData_.y(n));
// 		}
// 	}
// 
// 	return true;
// }

// From Sample::createTypeList()
// 	// Calculate average scattering lengths for sample
// 	boundCoherentAverageSquared_ = 0.0;
// 	boundCoherentSquaredAverage_ = 0.0;
// 	for (at1 = typeIndex_.first(); at1 != NULL; at1 = at1->next)
// 	{
// 		// Note: Divisor of 10.0 in calculation of bb converts from units of fm (1e-11 m) to barn (1e-12 m)
// 		cc = at1->fraction();
// 		bb = at1->isotope()->boundCoherent() * 0.1;
// 		boundCoherentAverageSquared_ += cc*bb;
// 		boundCoherentSquaredAverage_ += cc*bb*bb;
// 	}
// 	boundCoherentAverageSquared_ *= boundCoherentAverageSquared_;
// 	
// 	Messenger::print("--> Calculated average scattering lengths: <b>**2 = %f, <b**2> = %f\n", boundCoherentAverageSquared_, boundCoherentSquaredAverage_);

// // Setup pair correlations
// bool Sample::setupPairCorrelations(double volume, double range, double binWidth, Data2D& boxNormalisation, double rho)
// {
// 	// Get a nice sample name (i.e. no spaces, slashes etc.)
// 	Dnchar otherName, niceSampleName = name_;
// 	niceSampleName.replace(' ', '_');
// 	niceSampleName.replace('/', '_');
// 	double cc, bb;
// 
// 	// Construct S(Q) and weights matrices based on the typeIndex_ population
// 	int typeI, typeJ;
// 	Messenger::print("--> Creating S(Q) matrices (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());
// 	pairSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
// 	braggSQMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
// 	weightsMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
// 	scatteringMatrix_.initialise(typeIndex_.nItems(), typeIndex_.nItems(), true);
// 
// 	// Set names of elements in the S(Q) arrays, and calculate weights and self-scattering values
// 	Dnchar title;
// 	AtomTypeData* at1 = typeIndex_.first(), *at2;
// 	for (typeI=0; typeI<typeIndex_.nItems(); ++typeI, at1 = at1->next)
// 	{
// 		at2 = at1;
// 		for (typeJ=typeI; typeJ<typeIndex_.nItems(); ++typeJ, at2 = at2->next)
// 		{
// 			// Partial S(Q) and Bragg S(Q)
// 			title.sprintf("%s[%i]-%s[%i] (%s)", at1->name(), at1->isotope()->A(), at2->name(), at2->isotope()->A(), niceSampleName.get());
// 			pairSQMatrix_.ref(typeI,typeJ).setName(title.get());
// 			braggSQMatrix_.ref(typeI,typeJ).setName(title.get());
// 			
// 			// Store weighting factor for this partial
// 			// Note: Divisor of 100.0 in calculation of bb converts from units of fm (1e-11 m) to barn (1e-12 m) squared
// 			cc = at1->fraction() * at2->fraction();
// 			bb = at1->isotope()->boundCoherent() * at2->isotope()->boundCoherent() * 0.01;
// 			weightsMatrix_.ref(typeI,typeJ) = cc * bb;
// 			scatteringMatrix_.ref(typeI,typeJ) = bb;
// 		}
// 	}
// 
// 	// Total g(r)
// 	int nBins = range / binWidth;
// 	totalGR_.initialise(nBins);
// 	for (int n=0; n<nBins; ++n) totalGR_.setX(n, (n+0.5)*binWidth);
// 	totalGR_.setName(niceSampleName.get());
// 
// 	// Total, reference and difference data (set names only)
// 	totalFQ_.setName(niceSampleName.get());
// 	otherName.sprintf("%s (Ref)", niceSampleName.get());
// 	referenceData_.setName(otherName.get());
// 	otherName.sprintf("%s (Diff)", niceSampleName.get());
// 	differenceData_.setName(otherName.get());
// 	
// 	// Perform FT of reference data (if present)
// 	if (referenceData_.nPoints() > 0)
// 	{
// 		referenceDataFT_ = referenceData_;
// // 		referenceDataFT_.rebin();
// 		referenceDataFT_.transformLorch(rho, 0.025, 50.0, 0.5, 0.08, true);
// 	}
// 
// 	return true;
// }


// From Sample::finaliseReferenceData().	
// 		// Is data normalised?
// 	if (referenceDataNormalisation_ == Sample::AverageSquaredNormalisation)
// 	{
// 		Messenger::print("--> Removing normalisation (multiplying by <b>**2 = %f).\n", boundCoherentAverageSquared_);
// 		referenceData_.arrayY() *= boundCoherentAverageSquared_;
// 	}
// 	else if (referenceDataNormalisation_ == Sample::SquaredAverageNormalisation)
// 	{
// 		Messenger::print("--> Removing normalisation (multiplying by <b**2> = %f).\n", boundCoherentSquaredAverage_);
// 		referenceData_.arrayY() *= boundCoherentSquaredAverage_;
// 	}


// Execute post-processing stage
bool StructureFactor::postProcess(DUQ& duq, ProcessPool& procPool)
{
	// TODO Save stuff here
	return false;
}



// // Save all partial S(Q)
// void Configuration::saveSQ(const char* baseName)
// {
// 	LineParser parser;
// 	int typeI, typeJ, n;
// 
// 	for (typeI=0; typeI<usedAtomTypes_.nItems(); ++typeI)
// 	{
// 		for (typeJ=typeI; typeJ<usedAtomTypes_.nItems(); ++typeJ)
// 		{
// 			Dnchar filename(-1, "%s-unweighted-%s-%s.sq", baseName, usedAtomTypes_[typeI]->name(), usedAtomTypes_[typeJ]->name());
// 
// 			// Open file and check that we're OK to proceed writing to it
// 			Messenger::print("Writing S(Q) file '%s'...\n", filename.get());
// 
// 			parser.openOutput(filename, true);
// 			if (!parser.isFileGoodForWriting())
// 			{
// 				Messenger::error("Couldn't open file '%s' for writing.\n", filename.get());
// 				continue;
// 			}
// 			
// 			Data2D& pair = pairSQMatrix_.ref(typeI,typeJ);
// 			Data2D& bound = boundSQMatrix_.ref(typeI,typeJ);
// 			Data2D& unbound = unboundSQMatrix_.ref(typeI,typeJ);
// 			Data2D& bragg = braggSQMatrix_.ref(typeI,typeJ);
// 			Data2D& partial = partialSQMatrix_.ref(typeI,typeJ);
// 			parser.writeLineF("# Unweighted partial S(Q) for types %s and %s calculated on TODO\n", usedAtomTypes_[typeI]->name(), usedAtomTypes_[typeJ]->name());
// 			parser.writeLine("#       S(Q) contains full pair correlation (bound + unbound) and Bragg scattering (if calculated)\n");
// 			parser.writeLine("#   Bound(Q) contains only pair correlations between atoms joined by intramolecular bonds or angles\n");
// 			parser.writeLine("# Unbound(Q) contains only pair correlations between atoms *not* joined by intramolecular bonds or angles\n");
// 			parser.writeLine("#    Pair(Q) contains full pair correlations\n");
// 			parser.writeLine("#   Bragg(Q) contains correlations calculated from HKL indices\n");
// 			parser.writeLineF("# %-16s  %-16s  %-16s  %-16s  %-16s  %-16s\n", "Q, 1/Angstroms", "S(Q)", "Bound(Q)", "Unbound(Q)", "Pair(Q)", "Bragg(Q)");
// 			for (n = 0; n< pair.nPoints(); ++n)
// 			{
// 				if (n < bragg.nPoints()) parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e  %16.10e  %16.10e\n", partial.x(n), partial.y(n), bound.y(n), unbound.y(n), pair.y(n), bragg.y(n));
// 				else parser.writeLineF("%16.10e  %16.10e  %16.10e  %16.10e  %16.10e\n", partial.x(n), partial.y(n), bound.y(n), unbound.y(n), pair.y(n));
// 			}
// 			parser.closeFiles();
// 		}
// 	}
// 
// 	Dnchar filename(-1, "%s-unweighted-total.fq", baseName);
// 	totalFQ_.save(filename);
// }
