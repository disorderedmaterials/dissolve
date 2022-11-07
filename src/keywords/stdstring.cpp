// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/stdstring.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

StringKeyword::StringKeyword(std::string &data) : KeywordBase(typeid(this)), data_(data), default_(data) {}

/*
 * Data
 */

// Return reference to data
std::string &StringKeyword::data() { return data_; }
const std::string &StringKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool StringKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        data_ = parser.argsv(startArg);

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool StringKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_);
}

// Express as a tree node
SerialisedValue StringKeyword::serialise() const { return data_; }

// Read values from a tree node
void StringKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_ = node.as_string(); }

// Has not changed from initial value
bool StringKeyword::isDefault() const { return data_ == default_; }
