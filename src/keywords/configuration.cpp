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
Configuration *&ConfigurationKeyword::data() { return data_; }
Configuration *&ConfigurationKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ConfigurationKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ConfigurationKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ConfigurationKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Configuration (first argument)
    data_ = coreData.findConfiguration(parser.argsv(startArg));
    if (!data_)
        return Messenger::error("Error setting Configuration - no Configuration named '{}' exists.\n", parser.argsv(startArg));

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ConfigurationKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
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
