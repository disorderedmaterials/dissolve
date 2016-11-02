/*
	*** dUQ Module Interface
	*** src/modules/module.cpp
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

#include "modules/module.h"
#include "classes/configuration.h"
#include <cstring>

// Module Types
const char* ModuleTypeKeywords[Module::nModuleTypes] = { "Analysis", "Correlation", "Evolution", "Fit" };
Module::ModuleType Module::moduleType(const char* s)
{
	for (int n=0; n<Module::nModuleTypes; ++n) if (strcmp(s, ModuleTypeKeywords[n]) == 0) return (Module::ModuleType) n;
	return Module::nModuleTypes;
}
const char* Module::moduleType(Module::ModuleType mt)
{
	return ModuleTypeKeywords[mt];
}

// Constructor
Module::Module()
{
	instances_.own(this);
}
// Destructor
Module::~Module()
{
}

/*
 * Instances
 */

// Delete all instances of this Module
void Module::deleteInstances()
{
	while (instances_.first()) instances_.removeFirst();
}

/*
 * Basic Control
 */

// Frequency with which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq)
{
	frequency_ = freq;
}

// Frequency with which to run Module (relative to master simulation loop counter)
int Module::frequency()
{
	return frequency_;
}

// Set whether the Module is enabled
void Module::setEnabled(bool b)
{
	enabled_ = b;
}

// Return whether the Module is enabled
bool Module::enabled()
{
	return enabled_;
}

/*
 * Targets
 */

// Add Configuration target
void Module::addConfigurationTarget(Configuration* cfg)
{
	targetConfigurations_.add(cfg);
}

// Return number of targeted Configurations
int Module::nConfigurationTargets()
{
	return targetConfigurations_.nItems();
}

// Return first targeted Configuration
RefListItem<Configuration,bool>* Module::targetConfigurations()
{
	return targetConfigurations_.first();
}

// Add Sample target
void Module::addSampleTarget(Sample* sam)
{
	targetSamples_.add(sam);
}

// Return number of targeted Samples
int Module::nSampleTargets()
{
	return targetSamples_.nItems();
}

// Return first targeted Sample
RefListItem<Sample,bool>* Module::targetSamples()
{
	return targetSamples_.first();
}

/*
 * Variables
 */

// Add Variable to Module
void Module::addVariable(const char* varName, VariableValue defaultValue, const char* description)
{
	variables_.setVariable(varName, defaultValue, description, name());
}

// Retrieve variable from Module (bool)
bool Module::variableAsBool(const char* varName)
{
	return variables_.variableAsBool(varName);
}

// Retrieve variable from Module (int)
int Module::variableAsInt(const char* varName)
{
	return variables_.variableAsInt(varName);
}

// Retrieve variable from Module (double)
double Module::variableAsDouble(const char* varName)
{
	return variables_.variableAsDouble(varName);
}

// Retrieve variable from Module (char)
const char* Module::variableAsChar(const char* varName)
{
	return variables_.variableAsChar(varName);
}

// Retrieve Module variable from supplied Configuration, or get default value (bool)
bool Module::variableAsBool(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, name());
	return (var ? var->asBool() : variableAsBool(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (int)
int Module::variableAsInt(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, name());
	return (var ? var->asInt() : variableAsInt(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (double)
double Module::variableAsDouble(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, name());
	return (var ? var->asDouble() : variableAsDouble(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (char)
const char* Module::variableAsChar(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, name());
	return (var ? var->asChar() : variableAsChar(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value
void Module::setVariable(Configuration* cfg, const char* varName, VariableValue value)
{
	cfg->setModuleVariable(varName, value, "", name());
}

// Search for named variable in Module
Variable* Module::findVariable(const char* varName)
{
	return variables_.variable(varName, name());
}

// Return first defined Variable
Variable* Module::variables()
{
	return variables_.variables();
}

/*
 * Parallel Comms
 */

bool Module::broadcastVariables(ProcessPool& procPool)
{
	return variables_.broadcast(procPool);
}
