/*
	*** Calibration Module
	*** src/modules/calibration/calibration.h
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

#ifndef DISSOLVE_CALIBRATIONMODULE_H
#define DISSOLVE_CALIBRATIONMODULE_H

#include "module/module.h"

// Forward Declarations
/* none */

// Calibration Module
class CalibrationModule : public Module
{
	/*
	 * Module for testing various functions
	 */

	public:
	// Constructor
	CalibrationModule();
	// Destructor
	~CalibrationModule();


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
	// Modules upon which this Module depends to have run first
	const char* dependentModules();
	// Set up supplied dependent module (only if it has been auto-added)
	bool setUpDependentModule(Module* depMod);


	/*
	 * Options
	 */
	public:
	// IntraBroadening Fitting Targets
	enum IntraBroadeningFitTarget
	{
		IntraBroadeningTargetSQ,
		IntraBroadeningTargetGR,
		IntraBroadeningTargetBoth,
		nIntraBroadeningFitTargets
	};
	// Convert string to functional form
	static IntraBroadeningFitTarget intraBroadeningFitTarget(const char* s);
	// Return fit target text
	static const char* intraBroadeningFitTarget(IntraBroadeningFitTarget ft);
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);

	public:
	// Whether the Module has a processing stage
	bool hasProcessing();


	/*
	 * Members / Functions
	 */
	private:
	// RDF Module targets for IntraBroadening calibration
	RefList<Module,bool> intraBroadeningModules_;
	// NeutronSQ Module targets for IntraBroadening calibration
	RefList<Module,IntraBroadeningFitTarget> intraBroadeningReferences_;


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

// Interface Class for Complex Cost Functions
class CalibrationModuleCostFunctions
{
	public:
	// Constructor
	CalibrationModuleCostFunctions(Dissolve& dissolve, ProcessPool& procPool, RefList<Module,bool>& intraBroadeningModules, RefList<Module,CalibrationModule::IntraBroadeningFitTarget>& intraBroadeningReferences);

	private:
	// Dissolve Main Object
	Dissolve& dissolve_;
	// Target ProcessPool
	ProcessPool& processPool_;
	// RDFModule targets for IntraBroadening fitting
	RefList<Module,bool>& intraBroadeningModules_;
	// NeutronSQModule targets for IntraBroadening fitting
	RefList<Module,CalibrationModule::IntraBroadeningFitTarget>& intraBroadeningReferences_;

	public:
	// Cost function for intraBroadening minimisation
	double intraBroadeningCost(const Array<double>& alpha);
};

#endif

