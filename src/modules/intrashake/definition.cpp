/*
	*** IntraShake Module - Definition
	*** src/modules/intrashake/definition.cpp
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

#include "modules/intrashake/intrashake.h"

// Return name of module
const char* IntraShakeModule::name()
{
	return "IntraShake";
}

// Return brief description of module
const char* IntraShakeModule::brief()
{
	return "Perform Monte Carlo shakes on intramolecular terms within Molecules";
}

// Return instance type for module
Module::InstanceType IntraShakeModule::instanceType()
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int IntraShakeModule::nTargetableConfigurations()
{
	return -1;
}

// Whether the Module has a pre-processing stage
bool IntraShakeModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool IntraShakeModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool IntraShakeModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* IntraShakeModule::dependentModules()
{
	return "";
}

// Set up supplied dependent module (only if it has been auto-added)
bool IntraShakeModule::setUpDependentModule(Module* depMod)
{
	return true;
}
