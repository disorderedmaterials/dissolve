// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "items/list.h"
#include "keywords/configurationVector.h"
#include "keywords/store.h"
#include "math/sampledDouble.h"
#include "module/types.h"

// Forward Declarations
class Dissolve;
class Configuration;
class ModuleWidget;
class ModuleContext;

// Module
class Module : public Serialisable<const CoreData &>
{
    public:
    explicit Module(const ModuleTypes::ModuleType type);
    virtual ~Module();

    /*
     * Definition
     */
    protected:
    // Module type
    const ModuleTypes::ModuleType type_;
    // Name of Module
    std::string name_;

    public:
    // Return type of Module
    ModuleTypes::ModuleType type() const;
    // Set name of Module
    void setName(std::string_view name);
    // Return name of Module
    std::string_view name() const;

    /*
     * Keywords
     */
    protected:
    // Keywords recognised by Module
    KeywordStore keywords_;

    public:
    // Return list of recognised keywords
    KeywordStore &keywords();
    const KeywordStore &keywords() const;
    // Print valid keywords
    void printValidKeywords();

    /*
     * Control
     */
    protected:
    // Frequency at which to run Module (relative to layer execution count)
    int frequency_;
    // Whether the Module is enabled
    bool enabled_;
    // Whether the module runs on its targets if the targets haven't changed since last time
    bool executeIfTargetsUnchanged_{false};

    public:
    // Module execution result
    enum class ExecutionResult
    {
        Failed,
        Success,
        NotExecuted
    };
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
     * Processing
     */
    private:
    // Configuration on which the module was last run and its version counter
    std::map<const Configuration *, int> lastProcessedConfigurations_;

    private:
    // Get current target configurations
    std::pair<std::vector<const Configuration *>, int> getCurrentTargetConfigurations();
    // Check the current configurations targeted by the module
    ExecutionResult checkConfigurationTargets(GenericList &processingModuleData);
    // Run main processing
    virtual ExecutionResult process(ModuleContext &moduleContext) = 0;

    public:
    // Set target data
    virtual void setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                            const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap);
    // Run set-up stage
    virtual bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals = {});
    // Run main processing stage
    ExecutionResult executeProcessing(ModuleContext &moduleContext);

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

    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
