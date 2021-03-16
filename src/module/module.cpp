// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "module/module.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "keywords/configurationreflist.h"
#include "main/dissolve.h"

Module::Module(int nTargetConfiguration) : targetConfigurationsKeyword_(targetConfigurations_, nTargetConfiguration)
{
    frequency_ = 1;
    enabled_ = true;
    configurationLocal_ = true;

    // Set up basic keywords for the Module
    keywords_.add("HIDDEN", &targetConfigurationsKeyword_, "Configuration", "Set target configuration(s) for the module");
}

Module::~Module()
{
    // Need to remove our local keywords from the list before it gets destructed to avoid a double free
    keywords_.cut(&targetConfigurationsKeyword_);
}

/*
 * Definition
 */

// Set unique name of Module
void Module::setUniqueName(std::string_view uniqueName) { uniqueName_ = uniqueName; }

// Return unique name of Module
std::string_view Module::uniqueName() const { return uniqueName_; }

/*
 * Keywords
 */

// Return list of recognised keywords
KeywordList &Module::keywords() { return keywords_; }
const KeywordList &Module::keywords() const { return keywords_; };

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
KeywordBase::ParseResult Module::parseKeyword(LineParser &parser, Dissolve *dissolve, GenericList &targetList,
                                              std::string_view prefix)
{
    // The LineParser currently contains a parsed line from the input file...

    // Do we recognise the first item (the 'keyword')?
    KeywordBase *keyword = keywords_.find(parser.argsv(0));
    if (!keyword)
        return KeywordBase::Unrecognised;

    else
    {
        // Check the number of arguments we have against the min / max for the keyword
        if (!keyword->validNArgs(parser.nArgs() - 1))
            return KeywordBase::Failed;

        // All OK, so parse the keyword
        if (!keyword->read(parser, 1, dissolve->coreData()))
        {
            Messenger::error("Failed to parse arguments for Module keyword '{}'.\n", keyword->name());
            return KeywordBase::Failed;
        }
    }

    return KeywordBase::Success;
}

// Print valid keywords
void Module::printValidKeywords()
{
    Messenger::print("Valid keywords for '{}' Module are:\n", type());

    ListIterator<KeywordBase> keywordIterator(keywords_.keywords());
    while (KeywordBase *keyword = keywordIterator.iterate())
        Messenger::print("  {:30}  {}\n", keyword->name(), keyword->description());
}

/*
 * Control
 */

// Set frequency at which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq) { frequency_ = freq; }

// Return frequency at which to run Module (relative to master simulation loop counter)
int Module::frequency() const { return frequency_; }

// Return whether the Module should run this iteration
bool Module::runThisIteration(int iteration) const
{
    if ((frequency_ < 1) || (!enabled_))
        return false;
    else if ((iteration % frequency_) == 0)
        return true;
    else
        return false;
}

// Return short descriptive text relating frequency to supplied iteration number
std::string Module::frequencyDetails(int iteration) const
{
    // Check edge cases
    if (frequency_ < 0)
        return "NEGATIVE?";
    else if ((!enabled_) || (frequency_ == 0))
        return "disabled";
    else if (frequency_ == 1)
        return "every time";
    else if ((iteration % frequency_) == 0)
        return "this iteration";

    // Calculate number of steps necessary to get to next multiple of the frequency_
    auto nToGo = frequency_ - (iteration - frequency_ * (iteration / frequency_));
    if (nToGo == 1)
        return "next iteration";

    return fmt::format("in {} steps time", nToGo);
}

// Set whether the Module is enabled
void Module::setEnabled(bool b) { enabled_ = b; }

// Return whether the Module is enabled
bool Module::isEnabled() const { return enabled_; }

// Return whether the Module is disabled
bool Module::isDisabled() const { return !enabled_; }

/*
 * Targets
 */

// Add Configuration target
bool Module::addTargetConfiguration(Configuration *cfg)
{
    // Check how many Configurations we accept before we do anything else
    if ((nRequiredTargets() == Module::OneOrMoreTargets) || (targetConfigurations_.nItems() < nRequiredTargets()))
    {
        targetConfigurations_.append(cfg);
        keywords_.hasBeenSet("Configuration");
        return true;
    }
    else
    {
        if (nRequiredTargets() == Module::ZeroTargets)
            Messenger::error("Can't add Configuration '{}' as a target to Module '{}' since it doesn't accept any "
                             "such targets.\n",
                             cfg->name(), type());
        else
            Messenger::warn("Can't add Configuration '{}' as a target to Module '{}' since the maximum number ({}) "
                            "has already been reached.\n",
                            cfg->name(), type(), nRequiredTargets());
    }

    return false;
}

// Add Configuration targets
bool Module::addTargetConfigurations(const List<Configuration> &configs)
{
    if (nRequiredTargets() == Module::ZeroTargets)
        return Messenger::error("Module targets no configurations, so none will be set from the {} provided.\n",
                                configs.nItems());
    else if (nRequiredTargets() == Module::OneOrMoreTargets)
    {
        Messenger::print("Adding {} configurations as targets for module '{}'...\n", configs.nItems(), uniqueName());

        ListIterator<Configuration> configIterator(configs);
        while (Configuration *cfg = configIterator.iterate())
            if (!addTargetConfiguration(cfg))
                return Messenger::error("Failed to add configuration '{}' to module '{}'.\n", cfg->name(), uniqueName());
    }
    else if (nTargetConfigurations() == nRequiredTargets())
        return Messenger::error("Refusing to add any of the {} provided configurations as targets for the module '{}' "
                                "as it already has it's specified number ({}).\n",
                                configs.nItems(), uniqueName(), nRequiredTargets());
    else
    {
        auto spaces = nRequiredTargets() - nTargetConfigurations();
        Messenger::print("Adding up to {} configurations from the {} provided as targets for module '{}'...\n", spaces,
                         configs.nItems(), uniqueName());

        ListIterator<Configuration> configIterator(configs);
        while (Configuration *cfg = configIterator.iterate())
            if (!addTargetConfiguration(cfg))
                return Messenger::error("Failed to add configuration '{}' to module '{}'.\n", cfg->name(), uniqueName());
    }

    return true;
}

// Remove Configuration target
bool Module::removeTargetConfiguration(Configuration *cfg)
{
    if (!targetConfigurations_.contains(cfg))
        return Messenger::error("Can't remove Configuration '{}' from Module '{}' as it isn't currently a target.\n",
                                cfg->name(), uniqueName());

    targetConfigurations_.remove(cfg);

    return true;
}

// Return number of targeted Configurations
int Module::nTargetConfigurations() const { return targetConfigurations_.nItems(); }

// Return whether the number of targeted Configurations is valid
bool Module::hasValidNTargetConfigurations(bool reportError) const
{
    if (nRequiredTargets() == Module::OneOrMoreTargets)
    {
        auto valid = nTargetConfigurations() > 0;
        if (reportError && (!valid))
            Messenger::error("Module '{}' expects one or more configuration targets, but none have been provided.\n",
                             uniqueName());
        return valid;
    }
    else if (nRequiredTargets() == Module::ZeroTargets)
    {
        auto valid = (nTargetConfigurations() == 0);
        if (reportError && (!valid))
            Messenger::error("Module '{}' expects zero configuration targets, but {} ha{} been provided.\n", uniqueName(),
                             nTargetConfigurations(), nRequiredTargets() == 1 ? "s" : "ve");
        return valid;
    }
    else
    {
        auto valid = (nRequiredTargets() == nTargetConfigurations());
        if (reportError && (!valid))
            Messenger::error("Module '{}' expects exactly {} configuration {}, but {} ha{} been provided.\n", uniqueName(),
                             nRequiredTargets(), nRequiredTargets() == 1 ? "target" : "targets", nTargetConfigurations(),
                             nTargetConfigurations() == 1 ? "s" : "ve");
        return valid;
    }
}

// Return first targeted Configuration
const RefList<Configuration> &Module::targetConfigurations() const { return targetConfigurations_; }

// Return if the specified Configuration is in the targets list
bool Module::isTargetConfiguration(Configuration *cfg) const { return targetConfigurations_.contains(cfg); }

// Copy Configuration targets from specified Module
void Module::copyTargetConfigurations(Module *sourceModule)
{
    // First, check if this module actually accepts target Configurations
    if ((nRequiredTargets() < sourceModule->nTargetConfigurations()) && (nRequiredTargets() != Module::OneOrMoreTargets))
    {
        Messenger::warn("Dependent Module '{}' does not accept Configuration targets, but the source Module '{}' lists {}.\n",
                        type(), sourceModule->type());
        return;
    }
    for (auto cfg : sourceModule->targetConfigurations())
        addTargetConfiguration(cfg);
}

// Set whether this module is a local Module in a Configuration
void Module::setConfigurationLocal(bool b) { configurationLocal_ = b; }

// Return whether this module is a local Module in a Configuration
bool Module::configurationLocal() const { return configurationLocal_; }

/*
 * Processing
 */

// Run main processing
bool Module::process(Dissolve &dissolve, ProcessPool &procPool) { return false; }

// Run set-up stage
bool Module::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing stage
bool Module::executeProcessing(Dissolve &dissolve, ProcessPool &procPool)
{
    // Begin timer
    Timer timer;
    timer.start();

    // Run main processing routine
    auto result = process(dissolve, procPool);

    // Accumulate timing information
    timer.stop();
    processTimes_ += timer.secondsElapsed();

    return result;
}

/*
 * Timing
 */

// Return timing information (in seconds) for this Module
SampledDouble Module::processTimes() const { return processTimes_; }

// Read timing information through specified parser
bool Module::readProcessTimes(LineParser &parser) { return processTimes_.deserialise(parser); }

/*
 * GUI Widget
 */

// Return a new widget controlling this Module
ModuleWidget *Module::createWidget(QWidget *parent, Dissolve &dissolve) { return nullptr; }
