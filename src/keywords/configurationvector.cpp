// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/configurationvector.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"

ConfigurationVectorKeyword::ConfigurationVectorKeyword(std::vector<Configuration *> &data, int maxListSize)
    : KeywordBase(typeid(this)), data_(data)
{
    maxListSize_ = maxListSize;
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ConfigurationVectorKeyword::isDataEmpty() const { return data_.empty(); }

// Return reference to data vector
std::vector<Configuration *> &ConfigurationVectorKeyword::data() { return data_; }
const std::vector<Configuration *> &ConfigurationVectorKeyword::data() const { return data_; }

// Return maximum number of Configurations to allow in the list
int ConfigurationVectorKeyword::maxListSize() const { return maxListSize_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ConfigurationVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ConfigurationVectorKeyword::maxArguments() const { return 99; }

// Deserialise from supplied LineParser, starting at given argument offset
bool ConfigurationVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Each argument is the name of a Configuration that we will add to our list
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        Configuration *cfg = coreData.findConfiguration(parser.argsv(n));
        if (!cfg)
            return Messenger::error("Error defining Configuration targets - no Configuration named '{}' exists.\n",
                                    parser.argsv(n));

        // Check maximum size of list
        if ((maxListSize_ != -1) && (data_.size() >= maxListSize_))
            return Messenger::error("Too many configurations given to keyword. Maximum allowed is {}.\n", maxListSize_);

        // Check that the configuration isn't already in the list
        if (std::find(data_.begin(), data_.end(), cfg) != data_.end())
            return Messenger::error("Configuration '{}' is already present in the list.\n", cfg->name());

        data_.push_back(cfg);
    }

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool ConfigurationVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over list of Configuration
    std::string configurationString;
    for (Configuration *cfg : data_)
        configurationString += fmt::format("{}'{}'", configurationString.empty() ? "" : "  ", cfg->name());

    if (!parser.writeLineF("{}{}  {}\n", prefix, keywordName, configurationString))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Configuration in the contained data
void ConfigurationVectorKeyword::removeReferencesTo(Configuration *cfg)
{
    data_.erase(std::remove(data_.begin(), data_.end(), cfg), data_.end());
}
