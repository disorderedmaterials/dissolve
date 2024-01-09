// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/configuration.h"
#include "base/lineParser.h"
#include "classes/configuration.h"
#include "classes/coreData.h"

ConfigurationKeyword::ConfigurationKeyword(Configuration *&data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
Configuration *&ConfigurationKeyword::data() { return data_; }
Configuration *&ConfigurationKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool ConfigurationKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Configuration (first argument)
    data_ = coreData.findConfiguration(parser.argsv(startArg));
    if (!data_)
        return Messenger::error("Error setting Configuration - no Configuration named '{}' exists.\n", parser.argsv(startArg));

    return true;
}

// Serialise data to specified LineParser
bool ConfigurationKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_ && !parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_->name()))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Configuration in the contained data
void ConfigurationKeyword::removeReferencesTo(Configuration *cfg)
{
    if (data_ == cfg)
        data_ = nullptr;
}

// Express as a serialisable value
SerialisedValue ConfigurationKeyword::serialise() const
{
    // isDefault is checked before serialisation of keywords
    // so we have checked for the null pointer
    assert(data_);
    return data_->name();
}

// Read values from a serialisable value
void ConfigurationKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_ = coreData.findConfiguration(std::string_view(std::string(node.as_string())));
}
