/*
	*** Module Interface
	*** src/module/module.h
	Copyright T. Youngs 2012-2019

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
#include "genericitems/list.h"
#include "math/sampleddouble.h"
#include "keywords/group.h"
#include "keywords/list.h"
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


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	virtual Module* createInstance() const = 0;


	/*
	 * Definition
	 */
	protected:
	// Unique name of Module
	CharString uniqueName_;

	public:
	// Return type of Module
	virtual const char* type() const = 0;
	// Return category for Module
	virtual const char* category() const = 0;
	// Set unique name of Module
	void setUniqueName(const char* uniqueName);
	// Return unique name of Module
	const char* uniqueName() const;
	// Return brief description of Module
	virtual const char* brief() const = 0;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	virtual int nTargetableConfigurations() const = 0;


	/*
	 * Keywords
	 */
	protected:
	// Keywords recognised by Module
	KeywordList keywords_;

	
	public:
	// Return list of recognised keywords
	KeywordList& keywords();
	// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	KeywordBase::ParseResult parseKeyword(LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);
	// Print valid keywords
	void printValidKeywords();


	/*
	 * Initialisation
	 */
	public:
	// Perform any necessary initialisation for the Module
	virtual void initialise() = 0;


	/*
	 * Control
	 */
	protected:
	// Frequency at which to run Module (relative to layer execution count)
	int frequency_;
	// Whether the Module is enabled
	bool enabled_;

	public:
	// Set frequency at which to run Module (relative to layer execution count)
	void setFrequency(int freq);
	// Return frequency at which to run Module (relative to layer execution count)
	int frequency() const;
	// Return whether the Module should run this iteration
	bool runThisIteration(int iteration) const;
	// Return short descriptive text relating frequency to supplied iteration number
	const char* frequencyDetails(int iteration) const;
	// Set whether the Module is enabled
	void setEnabled(bool b);
	// Return whether the Module is enabled
	bool isEnabled() const;
	// Return whether the Module is disabled
	bool isDisabled() const;


	/*
	 * Targets
	 */
	protected:
	// Configurations that are targeted by this Module
	RefList<Configuration> targetConfigurations_;
	// Whether this module is a local Module in a Configuration 
	bool configurationLocal_;

	public:
	// Add Configuration target
	bool addTargetConfiguration(Configuration* cfg);
	// Add Configuration targets
	bool addTargetConfigurations(const List<Configuration>& configs);
	// Remove Configuration target
	bool removeTargetConfiguration(Configuration* cfg);
	// Return number of targeted Configurations
	int nTargetConfigurations() const;
	// Return targeted Configurations
	const RefList<Configuration>& targetConfigurations() const;
	// Return if the specified Configuration is in the targets list
	bool isTargetConfiguration(Configuration* cfg) const;
	// Copy Configuration targets from specified Module
	void copyTargetConfigurations(Module* sourceModule);
	// Set whether this module is a local Module in a Configuration
	void setConfigurationLocal(bool b);
	// Return whether this module is a local Module in a Configuration
	bool configurationLocal() const;


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

