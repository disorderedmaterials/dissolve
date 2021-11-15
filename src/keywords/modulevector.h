// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Module;

// Keyword with Module RefList data
class ModuleVectorKeyword : public KeywordBase
{
    public:
    ModuleVectorKeyword(std::vector<Module *> &data, int maxModules = -1);
    ModuleVectorKeyword(std::vector<Module *> &data, std::vector<std::string> allowedModuleTypes, int maxModules = -1);
    ~ModuleVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<Module *> &data_;
    // Module type(s) to allow
    std::vector<std::string> moduleTypes_;
    // Maximum number of modules to allow in list (-1 for any number)
    int maxModules_;

    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

    public:
    // Return the data vector
    std::vector<Module *> &data();
    const std::vector<Module *> &data() const;
    // Return the Module type(s) to allow
    const std::vector<std::string> &moduleTypes() const;
    // Return maximum number of Modules to allow in the list
    int maxModules() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Module in the contained data
    void removeReferencesTo(Module *module) override;
};
