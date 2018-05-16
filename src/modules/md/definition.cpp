/*
	*** MD Module - Definition
	*** src/modules/md/definition.cpp
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

#include "modules/md/md.h"

// Return name of module
const char* MDModule::name()
{
	return "MD";
}

// Return brief description of module
const char* MDModule::brief()
{
	return "Evolve a Configuration using molecular dynamics";
}

// Return instance type for module
Module::InstanceType MDModule::instanceType()
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int MDModule::nTargetableConfigurations()
{
	return 1;
}

// Modules upon which this Module depends to have run first
const char* MDModule::dependentModules()
{
	return "";
}

// Set up supplied dependent module (only if it has been auto-added)
bool MDModule::setUpDependentModule(Module* depMod)
{
	return true;
}
