/*
	*** RDF Module
	*** src/modules/rdf/rdf.h
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

#ifndef DISSOLVE_RDFMODULE_H
#define DISSOLVE_RDFMODULE_H

#include "module/module.h"
#include "classes/partialset.h"
#include "classes/datastore.h"
#include "math/pairbroadeningfunction.h"

// Forward Declarations
class Dissolve;
class ModuleGroup;
class PartialSet;
class Weights;

// RDF Module
class RDFModule : public Module
{
	public:
	// Constructor
	RDFModule();
	// Destructor
	~RDFModule();


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
	// Return type of module
	const char* type() const;
	// Return brief description of module
	const char* brief() const;
	// Return instance type for module
	InstanceType instanceType() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Options
	 */
	protected:
	// Set up keywords for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);

	public:
	// Partial Calculation Method enum
	enum PartialsMethod { AutoMethod, TestMethod, SimpleMethod, CellsMethod, nPartialsMethods };
	// Convert character string to PartialsMethod
	static PartialsMethod partialsMethod(const char* s);
	// Return character string for PartialsMethod
	static const char* partialsMethod(PartialsMethod pm);
	// Averaging scheme enum
	enum AveragingScheme { SimpleAveraging, ExponentialAveraging, nAveragingSchemes };
	// Convert character string to AveragingScheme
	static AveragingScheme averagingScheme(const char* s);
	// Return character string for AveragingScheme
	static const char* averagingScheme(AveragingScheme as);


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Members / Functions
	 */
	private:
	// Test data
	DataStore testData_;

	private:
	// Calculate partial g(r) in serial with simple double-loop
	bool calculateGRTestSerial(Configuration* cfg, PartialSet& partialSet);
	// Calculate partial g(r) with optimised double-loop
	bool calculateGRSimple(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet);
	// Calculate partial g(r) utilising Cell neighbour lists
	bool calculateGRCells(ProcessPool& procPool, Configuration* cfg, PartialSet& partialSet);
	// Perform averaging of named partial g(r)
	bool performGRAveraging(GenericList& moduleData, const char* name, const char* prefix, int nSetsInAverage, RDFModule::AveragingScheme averagingScheme);

	public:
	// (Re)calculate partial g(r) for the specified Configuration
	bool calculateGR(ProcessPool& procPool, Configuration* cfg, RDFModule::PartialsMethod method, bool allIntra, bool& alreadyUpToDate);
	// Calculate smoothed/broadened partial g(r) from supplied partials
	static bool calculateUnweightedGR(const PartialSet& originalgr, PartialSet& weightedgr, PairBroadeningFunction intraBroadening, int smoothing);
	// Return effective density for specified Module's target Configurations
	static double summedRho(Module* module, GenericList& processingModuleData);
	// Sum unweighted g(r) over the supplied Module's target Configurations
	static bool sumUnweightedGR(ProcessPool& procPool, Module* module, GenericList& processingModuleData, PartialSet& summedUnweightedGR);
	// Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
	static bool sumUnweightedGR(ProcessPool& procPool, Module* parentModule, ModuleGroup* moduleGroup, GenericList& processingModuleData, PartialSet& summedUnweightedGR);
	// Test supplied PartialSets against each other
	static bool testReferencePartials(PartialSet& setA, PartialSet& setB, double testThreshold);
	// Test calculated partial against supplied reference data
	static bool testReferencePartial(const PartialSet& partials, double testThreshold, const Data1D& testData, const char* typeIorTotal, const char* typeJ = NULL, const char* target = NULL);
	// Test calculated vs reference data (two source sets)
	static bool testReferencePartials(const DataStore& testData, double testThreshold, const PartialSet& partials, const char* prefix);
	// Test calculated vs reference data (two source sets)
	static bool testReferencePartials(const DataStore& testData, double testThreshold, const PartialSet& partialsA, const char* prefixA, const PartialSet& partialsB, const char* prefixB);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

