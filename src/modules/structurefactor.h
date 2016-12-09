/*
	*** Structure Factor Module
	*** src/modules/structurefactor.h
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

#ifndef DUQ_STRUCTUREFACTOR_H
#define DUQ_STRUCTUREFACTOR_H

#include "modules/module.h"
#include "classes/partialqset.h"

// Forward Declarations
/* none */

// Structure Factor Module
class StructureFactor : public Module
{
	/*
	 * Calculates partial and full structure factors for the Sample or Configuration in which the Module is placed.
	 * Partial RDFs according to atomtype isotopes are constructed, and combined into total F(Q).
	 */

	public:
	// Constructor
	StructureFactor();
	// Destructor
	~StructureFactor();


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
	 * Targets
	 */
	public:
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations();
	// Return the maximum number of Samples the Module can target (or -1 for any number)
	int nTargetableSamples();


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
	 * Static Members / Functions
	 */
	private:
	// List of S(Q) PartialSets for specific Configuration
	static List<PartialQSet> partialSets_;

	public:
	// Add or return new PartialSet for specified Configuration
	static PartialQSet* partialSet(Configuration* cfg);
	// Calculate unweighted S(Q) for the specified Configuration
	bool calculateUnweighted(Configuration* cfg, Data2D::WindowFunction windowFunction, ProcessPool& procPool);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data associated to module
	bool broadcastData(DUQ& duq, ProcessPool& procPool);
};

#endif
