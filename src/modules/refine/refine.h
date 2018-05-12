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

#ifndef DUQ_REFINEMODULE_H
#define DUQ_REFINEMODULE_H

#include "module/module.h"
#include "classes/data.h"
#include "classes/scatteringmatrix.h"

// Forward Declarations
class AtomType;
class PartialSet;

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
	// Potential Inversion Method Enum
	enum PotentialInversionMethod
	{
		DirectFourierPotentialInversion,	/* Invert delta S(Q) by direct FT, and treat as addition to potential */
		DirectGaussianPotentialInversion,	/* Invert delta S(Q) by approximating with Gaussians, and treat as addition to potential */
		PercusYevickPotentialInversion,		/* Use Percus-Yevick closure to generate potential from delta S(Q) */
		HypernettedChainPotentialInversion,	/* Use the hypernetted chain closure to generate potential from delta S(Q) */
		nPotentialInversionMethods
	};
	// Convert text string to PotentialInversionMethod
	static PotentialInversionMethod potentialInversionMethod(const char* s);
	// Convert PotentialInversionMethod to text string
	static const char* potentialInversionMethod(PotentialInversionMethod pim);
	// Matrix Augmentation Style
	enum MatrixAugmentationStyle
	{
		NoAugmentation,				/* Do not supplement the scattering matrix with any additional data */
		PartialsAugmentation,			/* Augment scattering matrix with individual partial S(Q), as EPSR does */
		nMatrixAugmentationStyles
	};
	// Convert text string to MatrixAugmentationStyle
	static MatrixAugmentationStyle matrixAugmentationStyle(const char* s);
	// Convert MatrixAugmentationStyle to text string
	static const char* matrixAugmentationStyle(MatrixAugmentationStyle mas);

	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run pre-processing stage
	bool preProcess(DUQ& duq, ProcessPool& procPool);
	// Run main processing
	bool process(DUQ& duq, ProcessPool& procPool);
	// Run post-processing stage
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
	// Create full scattering matrix
	bool createScatteringMatrix(DUQ& duq, const PartialSet& unweightedSQ, MatrixAugmentationStyle augmentationStyle, double augmentationParam);
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
	// Full scattering matrix containing reference dat
	ScatteringMatrix scatteringMatrix_;
	// Simulated data added as reference data
	List<Data> simulatedReferenceData_;


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, DUQ& dUQ);
};

#endif

