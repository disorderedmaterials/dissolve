// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "classes/coredata.h"
#include "keywords/data.h"
#include "module/module.h"

// Keyword with Module base class
class ModuleKeywordBase
{
    public:
    ModuleKeywordBase(std::string_view moduleType);
    virtual ~ModuleKeywordBase();

    /*
     * Target Module
     */
    private:
    // Target module type to allow
    std::string_view moduleType_;

    public:
    // Return target module type to allow
    std::string_view moduleType() const;
    // Set the target module
    virtual bool setModule(Module *module) = 0;
    // Return the current target module as the base class
    virtual const Module *baseModule() const = 0;

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    virtual int optionMask() const = 0;
};

// Keyword with Module
template <class M> class ModuleKeyword : public ModuleKeywordBase, public KeywordData<M *>
{
    public:
    ModuleKeyword(std::string_view moduleType, M *module = nullptr)
        : ModuleKeywordBase(moduleType), KeywordData<M *>(KeywordBase::ModuleData, module)
    {
    }
    ~ModuleKeyword() {}

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override { return 1; }
    // Return maximum number of arguments accepted
    int maxArguments() const override { return 1; }
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData)
    {
        Module *module = coreData.findModule(parser.argsv(startArg));
        if (!module)
            return Messenger::error("Module '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        return setModule(module);
    }
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        // No need to write the keyword if the module pointer is null
        if (KeywordData<M *>::data_ == nullptr)
            return true;

        if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), KeywordData<M *>::data_->uniqueName()))
            return false;

        return true;
    }

    /*
     * Target Module
     */
    public:
    // Set the target module
    bool setModule(Module *module)
    {
        if (!module)
            return false;

        // Attempt to case the supplied pointer to a Module of our type
        M *castModule = dynamic_cast<M *>(module);
        if (!castModule)
            return Messenger::error("Module '{}' given to keyword {} is of the wrong type ({}) - only a module of "
                                    "type '{}' can be accepted.\n",
                                    module->uniqueName(), KeywordBase::name(), module->type(), moduleType());

        KeywordData<M *>::data_ = castModule;

        KeywordData<M *>::set_ = true;

        return true;
    }
    // Return the current target module as the base class
    const Module *baseModule() const { return KeywordData<M *>::data_; }
    // Return the current target module
    M *module() const { return KeywordData<M *>::data_; }

    /*
     * Access to KeywordBase
     */
    public:
    // Return option mask for keyword
    int optionMask() const { return KeywordBase::optionMask(); }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Module in the contained data
    void removeReferencesTo(Module *module)
    {
        if (KeywordData<M *>::data_ == module)
            KeywordData<M *>::data_ = nullptr;
    }
};
