/*
	*** Dissolve Module Interface
	*** src/module/module.h
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

#ifndef DISSOLVE_MODULE_H
#define DISSOLVE_MODULE_H

#include "base/messenger.h"
#include "base/genericlist.h"
#include "math/sampleddouble.h"
#include "module/keywordlist.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class Configuration;
class ProcessPool;
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
	public:
	// Create instance of this module
	virtual Module* createInstance() = 0;
	// Return list of all created instances of this Module
	virtual List<Module>& instances() = 0;
	// Find instance with unique name specified
	Module* findInstance(const char* uniqueName);


	/*
	 * Definition
	 */
	protected:
	// Unique name of Module
	CharString uniqueName_;
	// Colour used in visual representation of Module
	int colour_[3];

	public:
	// Return type of Module
	virtual const char* type() const = 0;
	// Set unique name of Module
	void setUniqueName(const char* uniqueName);
	// Return unique name of Module
	const char* uniqueName() const;
	// Return brief description of Module
	virtual const char* brief() const = 0;
	// Return instance type for Module
	virtual InstanceType instanceType() const = 0;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	virtual int nTargetableConfigurations() const = 0;
	// Return colour used in visual representation of Module
	const int* colour() const;


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
	virtual int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix) = 0;

	public:
	// Return list of recognised keywords
	ModuleKeywordList& keywords();
	// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseKeyword(LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);
	// Print valid keywords
	void printValidKeywords();


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


	/*
	 * Processing
	 */
	private:
	// Run main processing
	virtual bool process(Dissolve& dissolve, ProcessPool& procPool) = 0;

	public:
	// Run set-up stage
	virtual bool setUp(Dissolve& dissolve, ProcessPool& procPool);
	// Run main processing stage
	bool executeProcessing(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * LogPoints
	 */
	protected:
	// Logpoint for instance-local data
	int logPoint_;
	// Logpoint reflecting time of last broadcast of instance-local data
	int broadcastPoint_;


	/*
	 * Timing
	 */
	private:
	// Accumulated timing information (in seconds) for this Module
	SampledDouble processTimes_;

	public:
	// Return timing information (in seconds) for this Module
	SampledDouble processTimes() const;
	// Read timing information through specified parser
	bool readProcessTimes(LineParser& parser);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	virtual ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

