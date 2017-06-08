/*
	*** Partials Module
	*** src/modules/partials.h
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
#include "classes/partialrset.h"

// Forward Declarations
class PartialQSet;
class PartialRSet;
class WeightsMatrix;

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
	// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList);


	/*
	 * Targets
	 */
	public:
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations();


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
	// Calculate partial RDFs with simple double-loop
	bool calculateSimple(Configuration* cfg, PartialRSet& partialSet, ProcessPool& procPool);

	public:
	// Weighting Type enum
	enum WeightingType { NoWeighting, NeutronWeighting };
	// (Re)calculate unweighted partials for the specified Configuration
	bool calculateUnweighted(Configuration* cfg, ProcessPool& procPool, int smoothing);
	// Calculate weighted partials (from existing partial) for the specified Configuration
	bool calculateWeighted(Configuration* cfg, PartialRSet& unweightedPartials, WeightsMatrix& weightsMatrix);
	// Generate S(Q) from supplied g(r)
	bool generateSQ(PartialRSet& partialgr, PartialQSet& partialsq, double qMax, double qDelta);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data associated to module
	bool broadcastData(DUQ& duq, ProcessPool& procPool);
};

#endif

