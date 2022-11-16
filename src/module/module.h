// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "genericitems/list.h"
#include "keywords/configurationvector.h"
#include "keywords/store.h"
#include "math/sampleddouble.h"

// Forward Declarations
class Dissolve;
class Configuration;
class ModuleWidget;
class QWidget;

// Module
class Module : public Serialisable<const CoreData &>
{
    public:
    explicit Module(std::string typeName);
    virtual ~Module();

    /*
     * Definition
     */
    protected:
    // Type name of module
    const std::string typeName_;
    // Name of Module
    std::string name_;

    public:
    // Return type of Module
    const std::string_view type() const;
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
     * Processing
     */
    private:
    // Run main processing
    virtual bool process(Dissolve &dissolve, const ProcessPool &procPool) = 0;

    public:
    // Set target data
    virtual void setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                            const std::map<std::string, std::vector<const Module *>> &moduleMap);
    // Run set-up stage
    virtual bool setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals = {});
    // Run main processing stage
    bool executeProcessing(Dissolve &dissolve, const ProcessPool &procPool);

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
     * Management
     */
    private:
    // Vector of all existing Modules
    static std::vector<Module *> instances_;

    public:
    // Return vector of all existing Modules
    const static std::vector<Module *> &instances();
    // Search for any instance of any module with the specified unique name
    static Module *find(std::string_view uniqueName);
    // Search for and return any instance(s) of the specified Module type
    static std::vector<Module *> allOfType(std::string_view moduleType);
    static std::vector<Module *> allOfType(std::vector<std::string> types);
    template <class M> static std::vector<M *> allOfType()
    {
        std::vector<M *> results;

        for (auto *module : instances_)
        {
            M *castModule = dynamic_cast<M *>(module);
            if (castModule)
                results.push_back(castModule);
        }

        return results;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &data) override;
};
