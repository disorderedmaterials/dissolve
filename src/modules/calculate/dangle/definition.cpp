/*
	*** Calculate Distance-Angle Module - Definition
	*** src/modules/calculate/dangle/definition.cpp
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

#include "modules/calculate/dangle/dangle.h"

// Return type of module
const char* CalculateDAngleModule::type() const	
{
	return "CalculateDAngle";
}

// Return brief description of module
const char* CalculateDAngleModule::brief() const
{
	return "Calculate distance/angle maps";
}

// Return instance type for module
Module::InstanceType CalculateDAngleModule::instanceType() const
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int CalculateDAngleModule::nTargetableConfigurations() const
{
	return 1;
}

// Modules upon which this Module depends to have run first
const char* CalculateDAngleModule::dependentModules()
{
	return "";
}

// Set up supplied dependent module (only if it has been auto-added)
bool CalculateDAngleModule::setUpDependentModule(Module* depMod)
{
	return true;
}
