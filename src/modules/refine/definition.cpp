/*
	*** Refine Module - Definition
	*** src/modules/refine/definition.cpp
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

#include "modules/refine/refine.h"

// Return name of module
const char* RefineModule::name()
{
	return "Refine";
}

// Return brief description of module
const char* RefineModule::brief()
{
	return "Refine interatomic potentials based on differences between calculated and reference data";
}

// Return instance type for module
Module::InstanceType RefineModule::instanceType()
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int RefineModule::nTargetableConfigurations()
{
	return 0;
}

// Modules upon which this Module depends to have run first
const char* RefineModule::dependentModules()
{
	// If the 'OnlyWhenStable' keyword is 'true', we require the Energy module
	// TODO This needs to be updated since Partials module doesn't exist. Just check for presence of EnergyStable while running instead?
	if (keywords_.asBool("OnlyWhenStable")) return "Energy";
	else return "";
}

// Set up supplied dependent module (only if it has been auto-added)
bool RefineModule::setUpDependentModule(Module* depMod)
{
	if (DUQSys::sameString(depMod->name(), "Energy"))
	{
		// Must add all Configuration targets in the associated Partials Module
		Module* partialsModule = dependentModule("Partials");
		if (!partialsModule) return false;
		depMod->copyTargetConfigurations(partialsModule);
	}
	return true;
}
