// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "classes/coreData.h"
#include "keywords/base.h"
#include "module/module.h"

// Keyword managing mutable Module base class
class MutableModuleKeywordBase : public KeywordBase
{
    public:
    explicit MutableModuleKeywordBase(ModuleTypes::ModuleType moduleType)
        : KeywordBase(typeid(this)), moduleType_(moduleType){};
    ~MutableModuleKeywordBase() override = default;

    /*
     * Target Module
     */
    private:
    // Target module type to allow
    ModuleTypes::ModuleType moduleType_;

    public:
    // Return target module type to allow
    ModuleTypes::ModuleType moduleType() const { return moduleType_; }
    // Set the target data
    virtual bool setData(Module *module) = 0;
    // Return the current target module as the base class
    virtual Module *module() = 0;
};

// Keyword managing Module
template <class M> class MutableModuleKeyword : public MutableModuleKeywordBase
{
    public:
    explicit MutableModuleKeyword(M *&data, ModuleTypes::ModuleType moduleType)
        : MutableModuleKeywordBase(moduleType), data_(data)
    {
    }
    ~MutableModuleKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    M *&data_;

    public:
    // Return reference to data
    M *&data() { return data_; }
    M *&data() const { return data_; }
    // Return base module pointer
    Module *module() override { return data_; }
    // Set data
    bool setData(Module *module) override
    {
        if (module == nullptr)
            data_ = nullptr;
        else
        {
            if (module->type() != moduleType() && module->type() != ModuleTypes::Any)
                return Messenger::error("Module '{}' given to keyword {} is of the wrong type ({}) - only a module of "
                                        "type '{}' can be accepted.\n",
                                        module->name(), KeywordBase::name(), ModuleTypes::moduleType(module->type()),
                                        moduleType());

            data_ = dynamic_cast<M *>(module);
            assert(data_);
        }

        return true;
    }

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        auto *module = Module::find(parser.argsv(startArg));
        if (!module)
            return Messenger::error("Module '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg),
                                    KeywordBase::name());

        return setData(module);
    }
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        // No need to write the keyword if the module pointer is null
        if (data_ == nullptr)
            return true;

        if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), data_->name()))
            return false;

        return true;
    }

    // Express as a serialisable value
    SerialisedValue serialise() const override
    {
        if (data_)
            return data_->name();
        return {};
    }

    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override
    {
        auto *module = Module::find(std::string(node.as_string()));
        if (!module)
            throw toml::syntax_error(fmt::format("Module '{}' given to keyword {} doesn't exist.\n",
                                                 std::string(node.as_string()), KeywordBase::name()),
                                     node.location());

        setData(module);
    }

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Module in the contained data
    void removeReferencesTo(Module *module) override
    {
        if (data_ == module)
            data_ = nullptr;
    }
};
