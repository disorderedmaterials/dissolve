// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Configuration;

// Keyword managing vector of Configurations
class ConfigurationVectorKeyword : public KeywordBase
{
    public:
    explicit ConfigurationVectorKeyword(std::vector<Configuration *> &data);
    ~ConfigurationVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<Configuration *> &data_;

    public:
    // Return reference to data vector
    std::vector<Configuration *> &data();
    const std::vector<Configuration *> &data() const;

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
    // Prune any references to the supplied Configuration in the contained data
    void removeReferencesTo(Configuration *cfg) override;

    public:
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
