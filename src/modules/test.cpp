/*
	*** Test Module
	*** src/modules/test.cpp
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

#include "modules/test.h"
#include "main/duq.h"
#include "classes/species.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Static Members
List<Module> Test::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
Test::Test() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s_%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	addVariable("Test", false);
	addVariable("Save", true);
	addVariable("StabilityWindow", 10);
	addVariable("StabilityThreshold", 0.01);
}

// Destructor
Test::~Test()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& Test::instances()
{
	return instances_;
}

// Create instance of this module
Module* Test::createInstance()
{
	return new Test;
}

/*
 * Definition
 */

// Return name of module
const char* Test::name()
{
	return "Test";
}

// Return brief description of module
const char* Test::brief()
{
	return "Calculate the total energy of a Configuration";
}

// Return instance type for module
Module::InstanceType Test::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool Test::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool Test::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool Test::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* Test::dependentModules()
{
	return "";
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int Test::nTargetableConfigurations()
{
	return -1;
}

// Return the maximum number of Samples the Module can target (or -1 for any number)
int Test::nTargetableSamples()
{
	return 0;
}

/*
 * Method
 */

// Perform setup tasks for module
bool Test::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool Test::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool Test::process(DUQ& duq, ProcessPool& procPool)
{
	return true;
}

// Execute post-processing stage
bool Test::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
