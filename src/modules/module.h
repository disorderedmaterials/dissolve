/*
	*** dUQ Module Interface
	*** src/modules/module.h
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

#ifndef DUQ_MODULE_H
#define DUQ_MODULE_H

#include "classes/configuration.h"
#include "base/variablelist.h"
#include "base/messenger.h"
#include "templates/list.h"

// Forward Declarations
class DUQ;

// Module
class Module : public ListItem<Module>, public VariableList
{
	public:
	// Constructor
	Module()
	{
		// Standard variables for all modules
		setVariable("frequency", 1);
	}
	// Destructor
	virtual ~Module()
	{
	}
	// Module Types
	enum ModuleType { AnalysisModule, CorrelationModule, EvolutionModule, FitModule, nModuleTypes };
	static ModuleType moduleType(const char* s);
	static const char* moduleType(ModuleType mt);


	/*
	 * Definition
	 */
	public:
	// Return name of module
	virtual const char* name() = 0;
	// Return brief description of module
	virtual const char* brief() = 0;
	// Return type of module
	virtual ModuleType type() = 0;
	// Number of Configurations that this module requires to run
	virtual int nConfigurationsRequired() = 0;


	/*
	 * Association
	 */
	private:
	// Configurations that explicitly reference using this module, and whether they are enabled
	RefList<Configuration,bool> associatedConfigurations_;

	private:
	// Find named configuration variable
	Variable* configurationVariable(Configuration& cfg, const char* varName)
	{
		// Find specified variable in Configuration
		Variable* var = cfg.variable(name(), varName);

		// If it is not there, raise a warning and return zero
		if (!var) Messenger::error("Variable '%s' for module '%s' not found in Configuration '%s'.\n", varName, name(), cfg.name());

		return var;
	}

	protected:
	// Return named int variable associated to specified Configuration (adding prefix for module automatically)
	int configurationVariableAsInt(Configuration& cfg, const char* varName)
	{
		// Find specified variable in Configuration
		Variable* var = configurationVariable(cfg, varName);

		return (var ? var->asInt() : 0);
	}
	// Return named double variable associated to specified Configuration (adding prefix for module automatically)
	double configurationVariableAsDouble(Configuration& cfg, const char* varName)
	{
		// Find specified variable in Configuration
		Variable* var = configurationVariable(cfg, varName);

		return (var ? var->asDouble() : 0.0);
	}
	// Return named string variable associated to specified Configuration (adding prefix for module automatically)
	const char* configurationVariableAsChar(Configuration& cfg, const char* varName)
	{
		// Find specified variable in Configuration
		Variable* var = configurationVariable(cfg, varName);

		return (var ? var->asChar() : "NULL");
	}


	/*
	 * Method
	 */
	private:
	// Perform setup tasks before executing main method
	bool setup(DUQ& duq, Configuration& config)
	{
		// Check number of configs supplied and required by the module
		if (nConfigurationsRequired() != 1)
		{
			Messenger::error("Number of configurations passed to module (1) is not the required number (%i).\n", nConfigurationsRequired());
			return false;
		}

		return true;
	}
	// Execute method on the specified config
	virtual bool execute(DUQ& duq, Configuration& config) = 0;
	// Perform finalisation tasks after executing main method
	bool finalise(DUQ& duq, Configuration& config)
	{
		return true;
	}

	public:
	// Run main module method
	bool run(DUQ& duq, Configuration& config)
	{
		// Setup module for calculation
		if (!setup(duq, config)) return false;

		// Run main method
		if (!execute(duq, config)) return false;

		// Finalise module after calculation
		if (!finalise(duq, config)) return false;

		return true;
	}
};

#endif

