/*
	*** dUQ Module Interface
	*** src/module/module.h
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

#ifndef DUQ_MODULE_H
#define DUQ_MODULE_H

#include "base/messenger.h"
#include "module/keywordlist.h"
#include "base/genericlist.h"
#include "templates/reflist.h"

// Forward Declarations
class DUQ;
class Configuration;
class ProcessPool;
class Data;
class ModuleList;
class ModuleWidget;
class QWidget;

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
	// Find instance with unique name specified
	Module* findInstance(const char* uniqueName);
	// Delete all instances of this Module
	void deleteInstances();


	/*
	 * Definition
	 */
	protected:
	// Unique name of Module
	CharString uniqueName_;
	// RefList of dependent Modules
	RefList<Module,bool> dependentModules_;

	public:
	// Return name of Module
	virtual const char* name() = 0;
	// Set unique name of Module
	void setUniqueName(const char* uniqueName);
	// Return unique name of Module
	const char* uniqueName();
	// Return brief description of Module
	virtual const char* brief() = 0;
	// Return instance type for Module
	virtual InstanceType instanceType() = 0;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	virtual int nTargetableConfigurations() = 0;
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
	// Set up supplied dependent module (only if it has been auto-added)
	virtual bool setUpDependentModule(Module* depMod) = 0;
	// Update targets for any auto-added dependent Modules with those of this Module
	bool updateDependentTargets(ModuleList& currentModuleList, bool autoAddDependents, GenericList& currentModuleData);


	/*
	 * Keywords
	 */
	protected:
	// Keywords recognised by Module
	ModuleKeywordList keywords_;

	protected:
	// Set up keywords for Module
	virtual void setUpKeywords() = 0;
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	virtual int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix) = 0;

	public:
	// Return list of recognised keywords
	ModuleKeywordList& keywords();
	// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix);


	/*
	 * Control
	 */
	protected:
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
	// Whether this module is a local Module in a Configuration 
	bool configurationLocal_;
	// Data that are targetted by this Module
	RefList<Data,bool> targetData_;

	public:
	// Add Configuration target
	bool addConfigurationTarget(Configuration* cfg);
	// Return number of targeted Configurations
	int nConfigurationTargets();
	// Return first targeted Configuration
	RefList<Configuration,bool>& targetConfigurations();
	// Return if the specified Configuration is in the targets list
	bool isTargetConfiguration(Configuration* cfg);
	// Copy Configuration targets from specified Module
	void copyTargetConfigurations(Module* sourceModule);
	// Set whether this module is a local Module in a Configuration
	void setConfigurationLocal(bool b);
	// Return whether this module is a local Module in a Configuration
	bool configurationLocal();
	// Add Data target
	bool addDataTarget(Data* data);
	// Return number of targeted Data
	int nDataTargets();
	// Return first targeted Data
	RefList<Data,bool>& targetData();


	/*
	 * Method
	 */
	public:
	// Perform set up tasks for Module
	virtual bool setUp(ProcessPool& procPool) = 0;
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
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	virtual ModuleWidget* createWidget(QWidget* parent, DUQ& dUQ);
};

#endif

