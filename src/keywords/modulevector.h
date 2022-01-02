// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Module;

// Keyword managing Module
class ModuleVectorKeyword : public KeywordBase
{
    public:
    ModuleVectorKeyword(std::vector<Module *> &data, std::optional<int> maxModules = std::nullopt);
    ModuleVectorKeyword(std::vector<Module *> &data, std::vector<std::string> allowedModuleTypes,
                        std::optional<int> maxModules = std::nullopt);
    ~ModuleVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<Module *> &data_;
    // Module type(s) to allow
    std::vector<std::string> moduleTypes_;
    // Maximum number of modules to allow
    std::optional<int> maxModules_;

    public:
    // Return the data vector
    std::vector<Module *> &data();
    const std::vector<Module *> &data() const;
    // Return the Module type(s) to allow
    const std::vector<std::string> &moduleTypes() const;
    // Return maximum number of Modules to allow
    std::optional<int> maxModules() const;

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Module in the contained data
    void removeReferencesTo(Module *module) override;
};
