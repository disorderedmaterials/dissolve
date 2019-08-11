/*
	*** Module Interface
	*** src/module/module.cpp
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

	// LogPoints
	logPoint_ = 0;
	broadcastPoint_ = 0;
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

// Create and return named keyword group
KeywordGroup* Module::addKeywordGroup(const char* name)
{
	// Check that a group with the specified name doesn't already exist
	KeywordGroup* group = NULL;
	for (group = keywordGroups_.first(); group != NULL; group = group->next) if (DissolveSys::sameString(name, group->name())) break;

	if (!group)
	{
		group = new KeywordGroup(keywords_);
		group->setName(name);
		keywordGroups_.own(group);
	}

	return group;
}

// Return list of recognised keywords
KeywordList& Module::keywords()
{
	return keywords_;
}

// Return list of defined keyword groups
const List<KeywordGroup>& Module::keywordGroups() const
{
	return keywordGroups_;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int Module::parseKeyword(LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	// The LineParser currently contains a parsed line from the input file...

	// Do we recognise the first item (the 'keyword')?
	KeywordBase* keyword = keywords_.find(parser.argc(0));
	if (!keyword) return -1;

	// We recognised the keyword - what should we try to do with it?
	if (keyword->type() == KeywordBase::ComplexData)
	{
		// It's a 'complex' keyword, one that either sets up a complicated object, or does something specific within the Module
		return parseComplexKeyword(keyword, parser, dissolve, targetList, prefix);
	}
	else
	{
		// It's a keyword that sets a simple POD or class
		// Check the number of arguments we have against the min / max for the keyword
		if ((parser.nArgs() - 1) < keyword->minArguments())
		{
			Messenger::error("Not enough arguments given to Module keyword '%s'.\n", keyword->keyword());
			return 0;
		}
		if ((keyword->maxArguments() >= 0) && ((parser.nArgs() - 1) > keyword->maxArguments()))
		{
			Messenger::error("Too many arguments given to Module keyword '%s'.\n", keyword->keyword());
			return 0;
		}

		// All OK, so parse the keyword
		if (!keyword->read(parser, 1, dissolve->coreData(), dissolve->worldPool()))
		{
			Messenger::error("Failed to parse arguments for Module keyword '%s'.\n", keyword->keyword());
			return 0;
		}
	}

	return true;
}

// Print valid keywords
void Module::printValidKeywords()
{
	Messenger::print("Valid keywords for '%s' Module are:\n", type());

	ListIterator<KeywordBase> keywordIterator(keywords_.keywords());
	while (KeywordBase* keyword = keywordIterator.iterate()) Messenger::print("  %30s  %s\n", keyword->keyword(), keyword->description());
}

/*
 * Control
 */

// Frequency with which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq)
{
	frequency_ = freq;
}

// Frequency with which to run Module (relative to master simulation loop counter)
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
	else if (frequency_ == 0) return "disabled";
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
bool Module::enabled() const
{
	return enabled_;
}

/*
 * Targets
 */

// Add Configuration target
bool Module::addTargetConfiguration(Configuration* cfg)
{
	// Check how many Configurations we accept before we do anything else
	if ((nTargetableConfigurations() == -1) || (targetConfigurations_.nItems() < nTargetableConfigurations()))
	{
		targetConfigurations_.append(cfg);
		return true;
	}
	else
	{
		if (nTargetableConfigurations() == 0) Messenger::error("Can't add Configuration '%s' as a target to Module '%s' since it doesn't accept any such targets.\n", cfg->name(), type());
		else Messenger::error("Can't add Configuration '%s' as a target to Module '%s' since the maximum number (%i) has already been reached.\n", cfg->name(), type(), nTargetableConfigurations());
	}

	return false;
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
	if ((nTargetableConfigurations() < sourceModule->nTargetConfigurations()) && (nTargetableConfigurations() != -1))
	{
		Messenger::warn("Dependent Module '%s' does not accept Configuration targets, but the source Module '%s' lists %i.\n", type(), sourceModule->type());
		return;
	}
	RefListIterator<Configuration> configIterator(sourceModule->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate()) addTargetConfiguration(cfg);
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
