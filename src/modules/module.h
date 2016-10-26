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

#include "base/variablelist.h"
#include "base/messenger.h"
#include "templates/reflist.h"

// Forward Declarations
class DUQ;
class Configuration;

// Module
class Module : public ListItem<Module>, public VariableList
{
	public:
	// Constructor
	Module()
	{
		instances_.own(this);
	}
	// Destructor
	virtual ~Module()
	{
	}
	// Module Types
	enum ModuleType { AnalysisModule, CorrelationModule, EvolutionModule, FitModule, nModuleTypes };
	static ModuleType moduleType(const char* s);
	static const char* moduleType(ModuleType mt);
	// Module Instance Styles
	enum InstanceType { UniqueInstance, SingleInstance, MultipleInstance };


	/*
	 * Instances
	 */
	private:
	// All created instances of this module
	List<Module> instances_;

	public:
	// Create instance of this module
	virtual Module* createInstance() = 0;
	// Delete all instances of this Module
	void deleteInstances()
	{
		while (instances_.first()) instances_.removeFirst();
	}


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
	// Return instance type for module
	virtual InstanceType instanceType() = 0;
	// Number of Configurations that this module requires to run (-1 for "one or more")
	virtual int nConfigurationsRequired() = 0;


	/*
	 * Basic Control
	 */
	private:
	// Frequency with which to run Module (relative to master simulation loop counter)
	int frequency_;
	// Whether the Module is enabled
	bool enabled_;

	public:
	// Frequency with which to run Module (relative to master simulation loop counter)
	void setFrequency(int freq)
	{
		frequency_ = freq;
	}
	// Frequency with which to run Module (relative to master simulation loop counter)
	int frequency()
	{
		return frequency_;
	}
	// Set whether the Module is enabled
	void setEnabled(bool b)
	{
		enabled_ = b;
	}
	// Return whether the Module is enabled
	bool enabled()
	{
		return enabled_;
	}


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

