// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/configuration.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"

ConfigurationKeyword::ConfigurationKeyword(Configuration *&data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
dissolve::any_ptr ConfigurationKeyword::data() const { return data_; }

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

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool ConfigurationKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_)
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_->name()))
            return false;
    }
    else if (!parser.writeLineF("{}{}  '?_?'\n", prefix, name()))
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
