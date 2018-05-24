/*
	*** Test Module - Definition
	*** src/modules/test/definition.cpp
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

#include "modules/test/test.h"

// Return name of module
const char* TestModule::name()
{
	return "Test";
}

// Return brief description of module
const char* TestModule::brief()
{
	return "Development Module";
}

// Return instance type for module
Module::InstanceType TestModule::instanceType()
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int TestModule::nTargetableConfigurations()
{
	return 0;
}

// Modules upon which this Module depends to have run first
const char* TestModule::dependentModules()
{
	return "";
}

// Set up supplied dependent module (only if it has been auto-added)
bool TestModule::setUpDependentModule(Module* depMod)
{
	return true;
}
