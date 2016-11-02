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
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "base/processpool.h"
#include "base/timer.h"
#include "math/matrix4.h"

/*
 * Constructor / Destructor
 */

// Constructor
StructureFactor::StructureFactor() : Module()
{
	// Setup variables / control parameters
	addVariable("QDelta", 0.05);
	addVariable("QMax", -1.0);
	addVariable("Bragg", "off");
	addVariable("BraggQDepBroadening", 0.0063);
	addVariable("BraggQIndepBroadening", 0.0);
	addVariable("QDepBroadening", 0.02, "FWHM of Gaussian for Q-dependent instrument broadening function (if required)");
	addVariable("QIndepepBroadening", 0.0, "FWHM of Gaussian for Q-independent instrument broadening function (if required)");
	addVariable("NormalisedToAvSq", false, "States that the reference F(Q) has been normalised to < b >**2");
	addVariable("NormalisedToSqAv", false, "States that the reference F(Q) has been normalised to < b**2 >");
	addVariable("SubtractSelf", false, "States that the self scattering (determined from the high-Q average) should be subtracted from the reference data");
}	
	
// Destructor
StructureFactor::~StructureFactor()
{
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

// Return type of module
Module::ModuleType StructureFactor::type()
{
	return Module::CalculationModule;
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
	return true;
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

	// Get target Configuration
	Configuration* cfg = targetConfigurations_.firstItem();

	// Retrieve control parameters from Configuration
	const double cutoffDistance = variableAsDouble("cutoffDistance") < 0.0 ? duq.pairPotentialRange() : variableAsDouble("cutoffDistance");
	const int nShakesPerAtom = variableAsInt("nShakesPerAtom");
	const bool braggOn = variableAsBool("Bragg");
	double stepSize = variableAsDouble("stepSize");
	const double termScale = 1.0;
	const double rRT = 1.0/(.008314472*cfg->temperature());

	// Print argument/parameter summary
// 	Messenger::print("StructureFactor: Cutoff distance is %f\n", cutoffDistance);
// 	Messenger::print("StructureFactor: Performing %i shake(s) per Atom\n", nShakesPerAtom);
// 	Messenger::print("StructureFactor: Translation step is %f Angstroms, target acceptance rate is %f.\n", stepSize, targetAcceptanceRate);

	// If there is a Sample target, then we calculate the weighted structure factors for it (using the supplied Configurations)
	// Otherwise takt eh Configuration target and calculate unweighted structure factors for it.

// From Sample::finaliseReferenceData(). => Should be 'Add Self SCattering, since we're trying to reproduce the reference data.
		// Subtract self-scattering background, calculated from high-Q region
// 	if (referenceDataSubtractSelf_)
// 	{
// 		Messenger::print("--> Subtracting self-scattering background from reference data...\n");
// 		double highQLevel = 0.0;
// 		// Take last 50% of points to calculate average
// 		for (int n=referenceData_.nPoints()*0.5; n<referenceData_.nPoints(); ++n) highQLevel += referenceData_.y(n);
// 		highQLevel /= (referenceData_.nPoints()*0.5);
// 		Messenger::print("--> High-Q average level is %f.\n", highQLevel);
// 		referenceData_.arrayY() -= highQLevel;
// 	}
// 	
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

// Execute post-processing stage
bool StructureFactor::postProcess(DUQ& duq, ProcessPool& procPool)
{
	// TODO Save stuff here
	return false;
}
