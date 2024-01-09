// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "module/module.h"

// Module Vector Keyword
class WeightedModuleVectorKeyword : public KeywordBase
{
    public:
    WeightedModuleVectorKeyword(std::vector<std::pair<Module *, double>> &data,
                                std::vector<ModuleTypes::ModuleType> allowedModuleTypes);
    ~WeightedModuleVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<std::pair<Module *, double>> &data_;
    // Module type(s) to allow
    std::vector<ModuleTypes::ModuleType> moduleTypes_;

    public:
    // Return the data vector
    std::vector<std::pair<Module *, double>> &data();
    const std::vector<std::pair<Module *, double>> &data() const;
    // Return the Module type(s) to allow
    const std::vector<ModuleTypes::ModuleType> &moduleTypes() const;

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

    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};
