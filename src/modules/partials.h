/*
	*** Partials Module
	*** src/modules/partials.h
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

#ifndef DUQ_PARTIALS_H
#define DUQ_PARTIALS_H

#include "modules/module.h"
#include "classes/partialrset.h"

// Forward Declarations
/* none */

// Partials Module
class Partials : public Module
{
	/*
	 * Calculates partial radial distribution functions for the Sample or Configuration in which the Module is placed.
	 * Partial RDFs according to atomtype isotopes are constructed individually, including separate bound/unbound terms.
	 */

	public:
	// Constructor
	Partials();
	// Destructor
	~Partials();


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
	// List of RDF PartialSets for specific Configuration
	static List<PartialRSet> partialSets_;

	private:
	// Calculate partial RDFs with simple double-loop
	static bool calculateSimple(PartialRSet* partialSet, ProcessPool& procPool);

	public:
	// Add or return new PartialSet for specified Configuration
	static PartialRSet* partialSet(Configuration* cfg);
	// (Re)calculate unweighted partials for the specified Configuration
	static bool calculateUnweighted(Configuration* cfg, ProcessPool& procPool, int method = 0);
};

#endif

