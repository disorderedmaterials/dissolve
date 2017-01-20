/*
	*** Skeleton Module
	*** src/modules/skeleton.cpp
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

#include "modules/skeleton.h"
#include "main/duq.h"
#include "base/sysfunc.h"

// Static Members
List<Module> Skeleton::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
Skeleton::Skeleton() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	options_.add("Skeleton", false);
}

// Destructor
Skeleton::~Skeleton()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& Skeleton::instances()
{
	return instances_;
}

// Create instance of this module
Module* Skeleton::createInstance()
{
	return new Skeleton;
}

/*
 * Definition
 */

// Return name of module
const char* Skeleton::name()
{
	return "Skeleton";
}

// Return brief description of module
const char* Skeleton::brief()
{
	return "Empty Module providing skeletal structure for new Module development";
}

// Return instance type for module
Module::InstanceType Skeleton::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool Skeleton::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool Skeleton::hasProcessing()
{
	return false;
}

// Whether the Module has a post-processing stage
bool Skeleton::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* Skeleton::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool Skeleton::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int Skeleton::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int Skeleton::nTargetableConfigurations()
{
	return 0;
}

/*
 * Method
 */

// Perform setup tasks for module
bool Skeleton::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool Skeleton::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool Skeleton::process(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute post-processing stage
bool Skeleton::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
