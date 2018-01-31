/*
	*** Molecular Dynamics Module
	*** src/modules/md/md.h
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

#ifndef DUQ_MDMODULE_H
#define DUQ_MDMODULE_H

#include "module/module.h"

// Forward Declarations
/* none */

// Molecular Dynamics Module
class MDModule : public Module
{
	/*
	 * Module for testing various functions
	 */

	public:
	// Constructor
	MDModule();
	// Destructor
	~MDModule();


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
	// Cap forces in Configuration
	int capForces(Configuration* cfg, double maxForceSq, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Determine timestep based on maximal force component
	double determineTimeStep(const Array<double>& fx, const Array<double>& fy, const Array<double>& fz);
};

#endif

