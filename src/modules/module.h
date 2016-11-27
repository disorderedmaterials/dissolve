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
class Sample;

// Module
class Module : public ListItem<Module>
{
	public:
	// Constructor
	Module();
	// Destructor
	virtual ~Module();
	// Module Instance Styles
	enum InstanceType { UniqueInstance, SingleInstance, MultipleInstance };


	/*
	 * Instances
	 */
	private:
	// Return list of all created instances of this Module
	virtual List<Module>& instances() = 0;

	public:
	// Create instance of this module
	virtual Module* createInstance() = 0;
	// Delete all instances of this Module
	void deleteInstances();


	/*
	 * Definition
	 */
	protected:
	// Unique name of Module
	Dnchar uniqueName_;
	// RefList of dependent Modules
	RefList<Module,bool> dependentModules_;

	public:
	// Return name of Module
	virtual const char* name() = 0;
	// Return unique name of Module
	const char* uniqueName();
	// Return brief description of Module
	virtual const char* brief() = 0;
	// Return instance type for Module
	virtual InstanceType instanceType() = 0;
	// Whether the Module has a pre-processing stage
	virtual bool hasPreProcessing() = 0;
	// Whether the Module has a processing stage
	virtual bool hasProcessing() = 0;
	// Whether the Module has a post-processing stage
	virtual bool hasPostProcessing() = 0;
	// Add dependent Module to this Module
	void addDependentModule(Module* module, bool autoAdded);
	// Return pointer for specified dependent Module
	Module* dependentModule(const char* name);
	// Modules upon which this Module depends to have run first
	virtual const char* dependentModules() = 0;
	// Update targets for any auto-added dependent Modules with those of this Module
	void updateDependentTargets();


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
	void setFrequency(int freq);
	// Frequency with which to run Module (relative to master simulation loop counter)
	int frequency();
	// Return whether the Module should run this iteration
	bool runThisIteration(int iteration);
	// Return short descriptive text relating frequency to supplied iteration number
	const char* frequencyDetails(int iteration);
	// Set whether the Module is enabled
	void setEnabled(bool b);
	// Return whether the Module is enabled
	bool enabled();


	/*
	 * Targets
	 */
	protected:
	// Configurations that are targeted by this Module
	RefList<Configuration,bool> targetConfigurations_;
	// Samples that are targeted by this module
	RefList<Sample,bool> targetSamples_;

	public:
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	virtual int nTargetableConfigurations() = 0;
	// Add Configuration target
	bool addConfigurationTarget(Configuration* cfg);
	// Return number of targeted Configurations
	int nConfigurationTargets();
	// Return first targeted Configuration
	RefList<Configuration,bool>& targetConfigurations();
	// Copy Configuration targets from specified Module
	void copyTargetConfigurations(Module* sourceModule);
	// Return the maximum number of Samples the Module can target (or -1 for any number)
	virtual int nTargetableSamples() = 0;
	// Add Sample target
	bool addSampleTarget(Sample* sam);
	// Return number of targeted Samples
	int nSampleTargets();
	// Return first targeted Sample
	RefList<Sample,bool>& targetSamples();
	// Copy Sample targets from specified Module
	void copyTargetSamples(Module* sourceModule);


	/*
	 * Variables
	 */
	private:
	// Module variables list
	VariableList variables_;

	public:
	// Add Variable to Module
	void addVariable(const char* varName, VariableValue defaultValue, const char* description = "");
	// Retrieve variable from Module (bool)
	bool variableAsBool(const char* varName);
	// Retrieve variable from Module (int)
	int variableAsInt(const char* varName);
	// Retrieve variable from Module (double)
	double variableAsDouble(const char* varName);
	// Retrieve variable from Module (char)
	const char* variableAsChar(const char* varName);
	// Retrieve variable from Module (Array<int>)
	Array<int>& variableAsIntArray(const char* varName);
	// Retrieve variable from Module (Array<double>)
	Array<double>& variableAsDoubleArray(const char* varName);
	// Retrieve Module variable in supplied Configuration (bool)
	bool variableAsBool(Configuration* cfg, const char* varName);
	// Retrieve Module variable in supplied Configuration (int)
	int variableAsInt(Configuration* cfg, const char* varName);
	// Retrieve Module variable in supplied Configuration (double)
	double variableAsDouble(Configuration* cfg, const char* varName);
	// Retrieve Module variable in supplied Configuration (char)
	const char* variableAsChar(Configuration* cfg, const char* varName);
	// Retrieve Module variable in supplied Configuration (Array<int>)
	Array<int>& variableAsIntArray(Configuration* cfg, const char* varName);
	// Retrieve Module variable in supplied Configuration (Array<double>)
	Array<double>& variableAsDoubleArray(Configuration* cfg, const char* varName);
	// Retrieve Module variable from supplied Configuration, or get default value
	void setVariable(Configuration* cfg, const char* varName, VariableValue value);
	// Append value to Module array variable in supplied Configuration
	bool appendVariable(Configuration* cfg, const char* varName, VariableValue value);
	// Search for named variable in Module
	Variable* findVariable(const char* varName);
	// Return first defined Variable
	Variable* variables();


	/*
	 * Method
	 */
	public:
	// Perform setup tasks for Module
	virtual bool setup(ProcessPool& procPool) = 0;
	// Execute pre-processing stage
	virtual bool preProcess(DUQ& duq, ProcessPool& procPool) = 0;
	// Execute method
	virtual bool process(DUQ& duq, ProcessPool& procPool) = 0;
	// Execute post-processing stage
	virtual bool postProcess(DUQ& duq, ProcessPool& procPool) = 0;


	/*
	 * LogPoints
	 */
	protected:
	// Logpoint for instance-local data
	int logPoint_;
	// Logpoint for static data, shared across all instances
	static int staticLogPoint_;
	// Logpoint reflecting time of last broadcast of instance-local data
	int broadcastPoint_;
	// Logpoint reflecting time of last broadcast of static data (shared across all instances)
	static int staticBroadcastPoint_;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast module variables
	bool broadcastVariables(ProcessPool& procPool);
	// Broadcast data associated to module
	virtual bool broadcastData(DUQ& duq, ProcessPool& procPool);
};

#endif

