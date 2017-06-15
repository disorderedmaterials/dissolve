/*
	*** Test Module
	*** src/modules/test.cpp
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

#include "modules/test.h"
#include "main/duq.h"
#include "base/sysfunc.h"

// Static Members
List<Module> TestModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
TestModule::TestModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Test", bool(false));
}

// Destructor
TestModule::~TestModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& TestModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* TestModule::createInstance()
{
	return new TestModule;
}

/*
 * Definition
 */

// Return name of module
const char* TestModule::name()
{
	return "Test";
}

// Return brief description of module
const char* TestModule::brief()
{
	return "Test Module";
}

// Return instance type for module
Module::InstanceType TestModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool TestModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool TestModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool TestModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* TestModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool TestModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int TestModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	if (DUQSys::sameString("AddTotalSQ", parser.argc(0)))
	{
		// Find the Module named by the first argument - it must be a Partials module in which the StructureFactor calculation is 'On'
		Module* partialsModule = ModuleList::findInstanceByUniqueName(parser.argc(1));
		if (!partialsModule)
		{
			Messenger::error("Specified Module target '%s' does not exist.", parser.argc(1));
			return false;
		}
		else if (!DUQSys::sameString(partialsModule->name(), "Partials"))
		{
			Messenger::error("Specified Module target '%s' is not a Partials module.", parser.argc(1));
			return false;
		}
		else if (!partialsModule->options().valueAsBool("StructureFactor"))
		{
			Messenger::error("Specified Module target '%s' does not calculate a structure factor.", parser.argc(1));
			return false;
		}

		// All fine, so add data to the list and load any reference data
		RefListItem<Module,XYData>* newSample = sampleData_.add(partialsModule);
		if (parser.hasArg(2) && !newSample->data.load(parser.argc(2))) return false;
	}
	else return -1;

	return true;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int TestModule::nTargetableConfigurations()
{
	return 0;
}

/*
 * Method
 */

// Perform setup tasks for module
bool TestModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool TestModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool TestModule::process(DUQ& duq, ProcessPool& procPool)
{
	// First, check that we can find the total structure factors that have been specified
	sampleData_.clear();
	
	RefListIterator<Module,XYData> sampleIterator(sampleData_);
	while (Module* module = sampleIterator.iterate())
	{
		// First, need to grab GenericList that will contain the target
		GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : duq.processingModuleData();
		
	}

	// Create weights matrix based on our defined 
	return true;
}

// Execute post-processing stage
bool TestModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
