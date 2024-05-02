// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/configurationVector.h"
#include "base/lineParser.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "templates/algorithms.h"

ConfigurationVectorKeyword::ConfigurationVectorKeyword(std::vector<Configuration *> &data)
    : KeywordBase(typeid(this)), data_(data)
{
}

/*
 * Data
 */

// Return reference to data vector
std::vector<Configuration *> &ConfigurationVectorKeyword::data() { return data_; }
const std::vector<Configuration *> &ConfigurationVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> ConfigurationVectorKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool ConfigurationVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Each argument is the name of a Configuration
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        auto *cfg = coreData.findConfiguration(parser.argsv(n));
        if (!cfg)
            return Messenger::error("Error defining Configuration targets - no Configuration named '{}' exists.\n",
                                    parser.argsv(n));

        // Check that the configuration isn't already present
        if (std::find(data_.begin(), data_.end(), cfg) != data_.end())
            return Messenger::error("Configuration '{}' has already been referenced.\n", cfg->name());

        data_.push_back(cfg);
    }

    return true;
}

// Serialise data to specified LineParser
bool ConfigurationVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName,
                             joinStrings(data_, "  ", [](const auto *cfg) { return fmt::format("'{}'", cfg->name()); }));
}

/*
 * Object Management
 */

// Prune any references to the supplied Configuration in the contained data
void ConfigurationVectorKeyword::removeReferencesTo(Configuration *cfg)
{
    data_.erase(std::remove(data_.begin(), data_.end(), cfg), data_.end());
}

// Express as a serialisable value
SerialisedValue ConfigurationVectorKeyword::serialise() const
{
    return fromVector(data_, [](const auto &cfg) { return std::string(cfg->name()); });
}

// Read values from a serialisable value
void ConfigurationVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [&coreData, this](const auto &name)
             {
                 auto *cfg = coreData.findConfiguration(std::string_view(std::string(name.as_string())));
                 if (!cfg)
                     throw toml::type_error(
                         fmt::format("Error defining Configuration targets - no Configuration named '{}' exists.\n",
                                     std::string(name.as_string())),
                         name.location());

                 // Check that the configuration isn't already present
                 if (std::find(data_.begin(), data_.end(), cfg) != data_.end())
                     throw toml::type_error(fmt::format("Configuration '{}' has already been referenced.\n", cfg->name()),
                                            name.location());

                 data_.push_back(cfg);
             });
}

// Has not changed from initial value
bool ConfigurationVectorKeyword::isDefault() const { return data_.empty(); }
