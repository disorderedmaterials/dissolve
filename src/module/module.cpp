/*
	*** Module Interface
	*** src/module/module.cpp
	Copyright T. Youngs 2012-2020

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

#include "main/dissolve.h"
#include "module/module.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
Module::Module()
{
	frequency_ = 1;
	enabled_ = true;
	configurationLocal_ = true;
}

// Destructor
Module::~Module()
{
}

/*
 * Definition
 */

// Set unique name of Module
void Module::setUniqueName(const char* uniqueName)
{
	uniqueName_ = uniqueName;
}

// Return unique name of Module
const char* Module::uniqueName() const
{
	return uniqueName_.get();
}

/*
 * Keywords
 */

// Return list of recognised keywords
KeywordList& Module::keywords()
{
	return keywords_;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
KeywordBase::ParseResult Module::parseKeyword(LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	// The LineParser currently contains a parsed line from the input file...

	// Do we recognise the first item (the 'keyword')?
	KeywordBase* keyword = keywords_.find(parser.argc(0));
	if (!keyword) return KeywordBase::Unrecognised;

	else
	{
		// Check the number of arguments we have against the min / max for the keyword
		if (!keyword->validNArgs(parser.nArgs() - 1)) return KeywordBase::Failed;

		// All OK, so parse the keyword
		if (!keyword->read(parser, 1, dissolve->coreData()))
		{
			Messenger::error("Failed to parse arguments for Module keyword '%s'.\n", keyword->name());
			return KeywordBase::Failed;
		}
	}

	return KeywordBase::Success;
}

// Print valid keywords
void Module::printValidKeywords()
{
	Messenger::print("Valid keywords for '%s' Module are:\n", type());

	ListIterator<KeywordBase> keywordIterator(keywords_.keywords());
	while (KeywordBase* keyword = keywordIterator.iterate()) Messenger::print("  %30s  %s\n", keyword->name(), keyword->description());
}

/*
 * Control
 */

// Set frequency at which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq)
{
	frequency_ = freq;
}

// Return frequency at which to run Module (relative to master simulation loop counter)
int Module::frequency() const
{
	return frequency_;
}

// Return whether the Module should run this iteration
bool Module::runThisIteration(int iteration) const
{
	if ((frequency_ < 1) || (!enabled_)) return false;
	else if ((iteration%frequency_) == 0) return true;
	else return false;
}

// Return short descriptive text relating frequency to supplied iteration number
const char* Module::frequencyDetails(int iteration) const
{
	static CharString result;

	if (frequency_ < 0) return "NEGATIVE?";
	else if ((!enabled_) || (frequency_ == 0)) return "disabled";
	else if (frequency_ == 1) return "every time";
	else if ((iteration%frequency_) == 0) return "this iteration";
	else
	{
		// Calculate number of steps necessary to get to next multiple of the frequency_
		int nToGo = frequency_ - (iteration - frequency_*(iteration/frequency_));
		if (nToGo == 1) return "next iteration";

		result.sprintf("in %i steps time", nToGo);
		return result.get();
	}
}

// Set whether the Module is enabled
void Module::setEnabled(bool b)
{
	enabled_ = b;
}

// Return whether the Module is enabled
bool Module::isEnabled() const
{
	return enabled_;
}

// Return whether the Module is disabled
bool Module::isDisabled() const
{
	return !enabled_;
}

/*
 * Targets
 */

// Add Configuration target
bool Module::addTargetConfiguration(Configuration* cfg)
{
	// Check how many Configurations we accept before we do anything else
	if ((nRequiredTargets() == Module::OneOrMoreTargets) || (targetConfigurations_.nItems() < nRequiredTargets()))
	{
		targetConfigurations_.append(cfg);
		return true;
	}
	else
	{
		if (nRequiredTargets() == Module::ZeroTargets) Messenger::error("Can't add Configuration '%s' as a target to Module '%s' since it doesn't accept any such targets.\n", cfg->name(), type());
		else Messenger::warn("Can't add Configuration '%s' as a target to Module '%s' since the maximum number (%i) has already been reached.\n", cfg->name(), type(), nRequiredTargets());
	}

	return false;
}

// Add Configuration targets
bool Module::addTargetConfigurations(const List<Configuration>& configs)
{
	if (nRequiredTargets() == Module::ZeroTargets) return Messenger::error("Module targets no configurations, so none will be set from the %i provided.\n", configs.nItems());
	else if (nRequiredTargets() == Module::OneOrMoreTargets)
	{
		Messenger::print("Adding %i configurations as targets for module '%s'...\n", configs.nItems(), uniqueName());

		ListIterator<Configuration> configIterator(configs);
		while (Configuration* cfg = configIterator.iterate()) if (!addTargetConfiguration(cfg)) return Messenger::error("Failed to add configuration '%s' to module '%s'.\n", cfg->name(), uniqueName());
	}
	else if (nTargetConfigurations() == nRequiredTargets()) return Messenger::error("Refusing to add any of the %i provided configurations as targets for the module '%s' as it already has it's specified number (%i).\n", configs.nItems(), uniqueName(), nRequiredTargets());
	else
	{
		int spaces = nRequiredTargets() - nTargetConfigurations();
		Messenger::print("Adding up to %i configurations from the %i provided as targets for module '%s'...\n", spaces, configs.nItems(), uniqueName());

		ListIterator<Configuration> configIterator(configs);
		while (Configuration* cfg = configIterator.iterate()) if (!addTargetConfiguration(cfg)) return Messenger::error("Failed to add configuration '%s' to module '%s'.\n", cfg->name(), uniqueName());
	}

	return true;
}

// Remove Configuration target
bool Module::removeTargetConfiguration(Configuration* cfg)
{
	if (!targetConfigurations_.contains(cfg)) return Messenger::error("Can't remove Configuration '%s' from Module '%s' as it isn't currently a target.\n", cfg->name(), uniqueName());

	targetConfigurations_.remove(cfg);

	return true;
}

// Return number of targeted Configurations
int Module::nTargetConfigurations() const
{
	return targetConfigurations_.nItems();
}

// Return whether the number of targeted Configurations is valid
bool Module::hasValidNTargetConfigurations(bool reportError) const
{
	if (nRequiredTargets() == Module::OneOrMoreTargets)
	{
		bool valid = nTargetConfigurations() > 0;
		if (reportError && (!valid)) Messenger::error("Module '%s' expects one or more configuration targets, but none have been provided.\n", uniqueName());
		return valid;
	}
	else if (nRequiredTargets() == Module::ZeroTargets)
	{
		bool valid = (nTargetConfigurations() == 0);
		if (reportError && (!valid)) Messenger::error("Module '%s' expects zero configuration targets, but %i ha%s been provided.\n", uniqueName(), nTargetConfigurations(), nRequiredTargets() == 1 ? "s" : "ve");
		return valid;
	}
	else
	{
		bool valid = (nRequiredTargets() == nTargetConfigurations());
		if (reportError && (!valid)) Messenger::error("Module '%s' expects exactly %i configuration %s, but %i ha%s been provided.\n", uniqueName(), nRequiredTargets(), nRequiredTargets() == 1 ? "target" : "targets", nTargetConfigurations(), nTargetConfigurations() == 1 ? "s" : "ve");
		return valid;
	}
}

// Return first targeted Configuration
const RefList<Configuration>& Module::targetConfigurations() const
{
	return targetConfigurations_;
}

// Return if the specified Configuration is in the targets list
bool Module::isTargetConfiguration(Configuration* cfg) const
{
	return targetConfigurations_.contains(cfg);
}

// Copy Configuration targets from specified Module
void Module::copyTargetConfigurations(Module* sourceModule)
{
	// First, check if this module actually accepts target Configurations
	if ((nRequiredTargets() < sourceModule->nTargetConfigurations()) && (nRequiredTargets() != Module::OneOrMoreTargets))
	{
		Messenger::warn("Dependent Module '%s' does not accept Configuration targets, but the source Module '%s' lists %i.\n", type(), sourceModule->type());
		return;
	}
	for(auto cfg : sourceModule->targetConfigurations()) addTargetConfiguration(cfg);
}

// Set whether this module is a local Module in a Configuration
void Module::setConfigurationLocal(bool b)
{
	 configurationLocal_ = b;
}

// Return whether this module is a local Module in a Configuration
bool Module::configurationLocal() const
{
	return configurationLocal_;
}

/*
 * Processing
 */

// Run main processing
bool Module::process(Dissolve& dissolve, ProcessPool& procPool)
{
	return false;
}

// Run set-up stage
bool Module::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	return true;
}

// Run main processing stage
bool Module::executeProcessing(Dissolve& dissolve, ProcessPool& procPool)
{
	// Begin timer
	Timer timer;
	timer.start();

	// Run main processing routine
	bool result = process(dissolve, procPool);

	// Accumulate timing information
	timer.stop();
	processTimes_ += timer.secondsElapsed();

	return result;
}

/*
 * Timing
 */

// Return timing information (in seconds) for this Module
SampledDouble Module::processTimes() const
{
	return processTimes_;
}

// Read timing information through specified parser
bool Module::readProcessTimes(LineParser& parser)
{
	static CoreData dummyData;
	return processTimes_.read(parser, dummyData);
}

/*
 * GUI Widget
 */

// Return a new widget controlling this Module
ModuleWidget* Module::createWidget(QWidget* parent, Dissolve& dissolve)
{
	return NULL;
}
