/*
	*** Refine Module
	*** src/modules/refine/refine.h
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

#ifndef DUQ_TESTMODULE_H
#define DUQ_TESTMODULE_H

#include "module/module.h"
#include "base/xydata.h"

// Forward Declarations
class AtomType;

// Refine Module
class RefineModule : public Module
{
	/*
	 * Module for testing various functions
	 */

	public:
	// Constructor
	RefineModule();
	// Destructor
	~RefineModule();


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
	// Set up supplied dependent module (only if it has been auto-added)
	bool setUpDependentModule(Module* depMod);


	/*
	 * Options
	 */
	public:
	// Potential Generation Type Enum
	enum PotentialGenerationType
	{
		DirectPotentialGeneration,		/* Invert delta S(Q) and treat as addition to potential */
		PercusYevickPotentialGeneration,	/* Use Percus-Yevick closure to generate potential from delta S(Q) */
		HypernettedChainPotentialGeneration,	/* Use the hypernetted chain closure to generate potential from delta S(Q) */
		nPotentialGenerationTypes
	};
	// Convert text string to PotentialGenerationType
	static PotentialGenerationType potentialGenerationType(const char* s);
	// Convert PotentialGenerationType to text string
	static const char* potentialGenerationType(PotentialGenerationType pgt);

	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix);


	/*
	 * Method
	 */
	public:
	// Perform set up tasks for module
	bool setUp(ProcessPool& procPool);
	// Execute pre-processing stage
	bool preProcess(DUQ& duq, ProcessPool& procPool);
	// Execute method on the specified config
	bool process(DUQ& duq, ProcessPool& procPool);
	// Execute post-processing stage
	bool postProcess(DUQ& duq, ProcessPool& procPool);


	/*
	 * Functions
	 */
	private:
	// Current data being fitted by modifyBondTerms
	XYData fitData_;
	// Starting position and maximum delta to allow on the x-intercept while fitting
	double xCentreStart_, xCentreDeltaLimit_;

	private:
	// Calculate c(r) from supplied S(Q)
	XYData calculateCR(const XYData& sq, double normFactor, double rMin, double rStep, double rMax, WindowFunction windowFunction = WindowFunction(), BroadeningFunction broadening = BroadeningFunction(), bool unbroaden = false);
	// Determine modification to bonds based on supplied delta g(r)
	bool modifyBondTerms(DUQ& duq, const XYData& deltaGR, AtomType* typeI, AtomType* typeJ, XYData& deltaBond);
	// Return value of fit equation given specified parameters
	inline double fitEquation(double x, double xCentre, double delta, double widthSquared, double AL, double AC, double AR);
	// Two-exponential, 5-parameter cost function for modifyBondTerms() fitting
	double costFunction2Exp(double x[], int n);
	// Three-exponential, 6-parameter cost function for modifyBondTerms() fitting
	double costFunction3Exp(double x[], int n);
	// Sum fitting equation with the specified parameters into the specified XYData
	void sumFitEquation(XYData& target, double xCentre, double delta, double widthSquared, double AL, double AC, double AR);


	/*
	 * Local Data
	 */
	private:
	// Weights matrix for partials / target data
	Array2D<double> partialWeights_;


	/*
	 * Data Access
	 */
	public:
	// Reap data for this Module instance
	void reapData();


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, DUQ& dUQ);
};

#endif

