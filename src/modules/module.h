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
class ProcessPool;

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
	enum ModuleType { AnalysisModule, CalculationModule, EvolutionModule, FitModule, nModuleTypes };
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
	// Return name of Module
	virtual const char* name() = 0;
	// Return brief description of Module
	virtual const char* brief() = 0;
	// Return type of Module
	virtual ModuleType type() = 0;
	// Return instance type for Module
	virtual InstanceType instanceType() = 0;
	// Whether the Module has a pre-processing stage
	virtual bool hasPreProcessing() = 0;
	// Whether the Module has a processing stage
	virtual bool hasProcessing() = 0;
	// Whether the Module has a post-processing stage
	virtual bool hasPostProcessing() = 0;


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
	public:
	// Perform setup tasks for module
	virtual bool setup(ProcessPool& procPool) = 0;
	// Execute pre-processing stage
	virtual bool preProcess(DUQ& duq, ProcessPool& procPool) = 0;
	// Execute method on the specified config
	virtual bool process(DUQ& duq, ProcessPool& procPool) = 0;
	// Execute post-processing stage
	virtual bool postProcess(DUQ& duq, ProcessPool& procPool) = 0;
};

#endif

