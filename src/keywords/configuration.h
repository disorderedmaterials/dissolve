// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Configuration;

// Keyword with Configuration Data
class ConfigurationKeyword : public KeywordBase
{
    public:
    explicit ConfigurationKeyword(Configuration *&data);
    ~ConfigurationKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Configuration *&data_;

    public:
    // Return reference to data
    Configuration *&data();
    Configuration *&data() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;

    /*
     * Object Management
     */
    protected:
    // Prune any references to the supplied Configuration in the contained data
    void removeReferencesTo(Configuration *cfg) override;
};
