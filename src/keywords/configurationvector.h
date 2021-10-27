// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Configuration;

// Keyword with Configuration Vector Data
class ConfigurationVectorKeyword : public KeywordBase
{
    public:
    ConfigurationVectorKeyword(std::vector<Configuration *> &cfgs, int maxListSize);
    ~ConfigurationVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data vector
    std::vector<Configuration *> &data_;
    // Maximum number of Configurations to allow in the list (or -1 for any number)
    int maxListSize_;

    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

    public:
    // Return reference to data vector
    std::vector<Configuration *> &data();
    const std::vector<Configuration *> &data() const;
    // Return maximum number of Configurations to allow in the list
    int maxListSize() const;

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
    // Prune any references to the supplied Configuration in the contained data
    void removeReferencesTo(Configuration *cfg) override;
};
