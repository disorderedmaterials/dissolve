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
		// Accessible variables for all modules
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
	// Create instance of this module
	virtual Module* createInstance() = 0;


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
	 * Targets
	 */
	protected:
	// Configurations that are targeted by this module
	RefList<Configuration,bool> targetConfigurations_;

	public:
	// Add Configuration target
	void addConfigurationTarget(Configuration* cfg)
	{
		targetConfigurations_.add(cfg);
	}


	/*
	 * Method
	 */
	private:
	// Perform setup tasks before executing main method
	bool setup(DUQ& duq)
	{
		// Check number of configs supplied and required by the module
		if (nConfigurationsRequired() != targetConfigurations_.nItems())
		{
			Messenger::error("Number of configurations targeted by module (%i) is not the required number (%i).\n", targetConfigurations_.nItems(), nConfigurationsRequired());
			return false;
		}

		return true;
	}
	// Execute method on the specified config
	virtual bool execute(DUQ& duq) = 0;
	// Perform finalisation tasks after executing main method
	bool finalise(DUQ& duq)
	{
		return true;
	}

	public:
	// Run main module method
	bool run(DUQ& duq)
	{
		// Setup module for calculation
		if (!setup(duq)) return false;

		// Run main method
		if (!execute(duq)) return false;

		// Finalise module after calculation
		if (!finalise(duq)) return false;

		return true;
	}
};

#endif

