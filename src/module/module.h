// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "genericitems/list.h"
#include "keywords/configurationreflist.h"
#include "keywords/group.h"
#include "keywords/list.h"
#include "math/sampleddouble.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class Configuration;
class ModuleList;
class ModuleWidget;
class QWidget;

// Module
class Module : public ListItem<Module>
{
    public:
    Module(int nTargetConfigurations);
    virtual ~Module();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    virtual Module *createInstance() const = 0;

    /*
     * Definition
     */
    protected:
    // Unique name of Module
    std::string uniqueName_;

    public:
    // Target Configurations Enum
    enum TargetConfigurationNumber
    {
        OneOrMoreTargets = -1,
        ZeroTargets = 0,
        ExactlyOneTarget = 1
    };
    // Return type of Module
    virtual std::string_view type() const = 0;
    // Return category for Module
    virtual std::string_view category() const = 0;
    // Set unique name of Module
    void setUniqueName(std::string_view uniqueName);
    // Return unique name of Module
    std::string_view uniqueName() const;
    // Return brief description of Module
    virtual std::string_view brief() const = 0;
    // Return the number of Configuration targets this Module requires
    virtual int nRequiredTargets() const = 0;

    /*
     * Keywords
     */
    protected:
    // Keywords recognised by Module
    KeywordList keywords_;
    // Target configurations keyword
    ConfigurationRefListKeyword targetConfigurationsKeyword_;

    public:
    // Return list of recognised keywords
    KeywordList &keywords();
    const KeywordList &keywords() const;
    // Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
    KeywordBase::ParseResult parseKeyword(LineParser &parser, Dissolve *dissolve, GenericList &targetList,
                                          std::string_view prefix);
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
    std::string frequencyDetails(int iteration) const;
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
    bool addTargetConfiguration(Configuration *cfg);
    // Add Configuration targets
    bool addTargetConfigurations(const List<Configuration> &configs);
    // Remove Configuration target
    bool removeTargetConfiguration(Configuration *cfg);
    // Return number of targeted Configurations
    int nTargetConfigurations() const;
    // Return whether the number of targeted Configurations is valid
    bool hasValidNTargetConfigurations(bool reportError = false) const;
    // Return targeted Configurations
    const RefList<Configuration> &targetConfigurations() const;
    // Return if the specified Configuration is in the targets list
    bool isTargetConfiguration(Configuration *cfg) const;
    // Copy Configuration targets from specified Module
    void copyTargetConfigurations(Module *sourceModule);
    // Set whether this module is a local Module in a Configuration
    void setConfigurationLocal(bool b);
    // Return whether this module is a local Module in a Configuration
    bool configurationLocal() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    virtual bool process(Dissolve &dissolve, ProcessPool &procPool) = 0;

    public:
    // Run set-up stage
    virtual bool setUp(Dissolve &dissolve, ProcessPool &procPool);
    // Run main processing stage
    bool executeProcessing(Dissolve &dissolve, ProcessPool &procPool);

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
    bool readProcessTimes(LineParser &parser);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    virtual ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
