/*
	*** Partials Module
	*** src/modules/partials/partials.h
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

#ifndef DUQ_PARTIALS_H
#define DUQ_PARTIALS_H

#include "modules/module.h"
#include "classes/partialset.h"
#include "classes/braggpeak.h"

// Forward Declarations
class PartialSet;
class Weights;

// Partials Module
class PartialsModule : public Module
{
	/*
	 * Calculates partial radial distribution functions for the Sample or Configuration in which the Module is placed.
	 * Partial RDFs according to atomtype isotopes are constructed individually, including separate bound/unbound terms.
	 * If a weighting scheme is specified with the Weights option, the weighted partials are also calculated.
	 */

	public:
	// Constructor
	PartialsModule();
	// Destructor
	~PartialsModule();


	/*
	 * Instances
	 */
	protected:
	// List of all instances of this Module type
	static List<Module> instances_;

	public:
	// Return list of all created instances of this Module
	List<Module>& instances();
	// Create instance of this module
	Module* createInstance();


	/*
	 * Definition
	 */
	public:
	// Return name of module
	const char* name();
	// Return brief description of module
	const char* brief();
	// Return instance type for module
	InstanceType instanceType();
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations();
	// Whether the Module has a pre-processing stage
	bool hasPreProcessing();
	// Whether the Module has a processing stage
	bool hasProcessing();
	// Whether the Module has a post-processing stage
	bool hasPostProcessing();
	// Modules upon which this Module depends to have run first
	const char* dependentModules();
	// Setup supplied dependent module (only if it has been auto-added)
	bool setupDependentModule(Module* depMod);


	/*
	 * Options
	 */
	protected:
	// Setup options for Module
	void setupOptions();

	public:
	// Partial Calculation Method enum
	enum PartialsMethod { AutoMethod, TestMethod, SimpleMethod, CellsMethod, nPartialsMethods };
	// Convert character string to PartialsMethod
	static PartialsMethod partialsMethod(const char* s);
	// Return character string for PartialsMethod
	static const char* partialsMethod(PartialsMethod pm);
	// Weighting Type enum
	enum WeightingType { NoWeighting, NeutronWeighting, nWeightingTypes };
	// Convert character string to WeightingType
	static WeightingType weightingType(const char* s);
	// Return character string for WeightingType
	static const char* weightingType(WeightingType wt);
	// Normalisation Type enum
	enum NormalisationType { NoNormalisation, AverageOfSquaresNormalisation, SquareOfAverageNormalisation, nNormalisationTypes };
	// Convert character string to NormalisationType
	static NormalisationType normalisationType(const char* s);
	// Return character string for NormalisationType
	static const char* normalisationType(NormalisationType nt);
	// Bragg Broadening enum
	enum BraggBroadening { NoBroadening, GaussianBroadening, nBroadeningTypes };
	// Convert character string to BraggBroadening
	static BraggBroadening braggBroadening(const char* s);
	// Return character string for BraggBroadening
	static const char* braggBroadening(BraggBroadening bt);
	// Averaging scheme enum
	enum AveragingScheme { SimpleAveraging, ExponentialAveraging, nAveragingSchemes };
	// Convert character string to AveragingScheme
	static AveragingScheme averagingScheme(const char* s);
	// Return character string for AveragingScheme
	static const char* averagingScheme(AveragingScheme as);

	public:
	// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList);


	/*
	 * Method
	 */
	public:
	// Perform setup tasks for module
	bool setup(ProcessPool& procPool);
	// Execute pre-processing stage
	bool preProcess(DUQ& duq, ProcessPool& procPool);
	// Execute method on the specified config
	bool process(DUQ& duq, ProcessPool& procPool);
	// Execute post-processing stage
	bool postProcess(DUQ& duq, ProcessPool& procPool);


	/*
	 * Members / Functions
	 */
	private:
	// Test supplied PartialSets against each other
	bool testReferencePartials(PartialSet& setA, PartialSet& setB, double testThreshold);
	// Test reference data against calculated PartialSet set
	bool testReferencePartials(GenericList& sourceModuleData, PartialSet& partialgr, const char* dataPrefix, double testThreshold);
	// Calculate partial RDFs in serial with simple double-loop
	bool calculateTest(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet);
	// Calculate partial RDFs with optimised double-loop
	bool calculateSimple(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet);
	// Calculate partial RDFs utilising Cell neighbour lists
	bool calculateCells(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet);
	// Perform averaging of specifed partials
	bool performAveraging(GenericList& moduleData, const char* name, const char* prefix, int nSetsInAverage, PartialsModule::AveragingScheme averagingScheme);

	public:
	// (Re)calculate unweighted partials for the specified Configuration
	bool calculateUnweightedGR(ProcessPool& procPool, Configuration* cfg, PartialsModule::PartialsMethod method, bool allIntra, int smoothing);
	// Calculate weighted partials from supplied unweighted partials
	bool calculateWeightedGR(PartialSet& unweightedPartials, PartialSet& weightedPartials, Weights& weights);
	// Calculate unweighted S(Q) from supplied unweighted g(r)
	bool calculateUnweightedSQ(ProcessPool& procPool, Configuration* cfg, double qMin, double qDelta, double qMax, double rho, XYData::WindowFunction wf, double qDepBroadening, double qIndepBroadening, bool braggOn);
	// Calculate weighted S(Q) from supplied unweighted S(Q)
	bool calculateWeightedSQ(PartialSet& unweightedsq, PartialSet& weightedsq, Weights& weights, PartialsModule::NormalisationType normalisation);
	// Calculate Bragg terms for specified Configuration
	bool calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, double braggQMin, double braggQResolution, double braggQMax, double braggMultiplicity);
	// Calculate unweighted Bragg partials from calculated peak data
	bool calculateUnweightedBraggSQ(ProcessPool& procPool, Configuration* cfg, Array<BraggPeak>& braggPeaks, PartialSet& partialsq, PartialsModule::BraggBroadening broadeningType);


	/*
	 * Data Access
	 */
	public:
	// Reap data for this Module instance
	void reapData();
};

#endif

