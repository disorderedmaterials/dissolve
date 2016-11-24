/*
	*** AtomShake Module
	*** src/modules/atomshake.h
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

#ifndef DUQ_ATOMSHAKEMODULE_H
#define DUQ_ATOMSHAKEMODULE_H

#include "modules/module.h"

// Forward Declarations
/* none */

// AtomShake Module
class AtomShake : public Module
{
	/*
	 * Performs a traditional atom-by-atom Monte Carlo cycle over all atoms in a configuration
	 */

	public:
	// Constructor
	AtomShake();
	// Destructor
	~AtomShake();


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
	 * Parallel Comms
	 */
	public:
	// Broadcast data associated to module
	bool broadcastData(ProcessPool& procPool, int rootRank);
};

#endif

