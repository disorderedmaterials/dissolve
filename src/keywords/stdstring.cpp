// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/stdstring.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

StringKeyword::StringKeyword(std::string &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
std::string &StringKeyword::data() { return data_; }
const std::std::any_ptr StringKeyword::data() { return data_; }
std::string StringKeyword::toString() const { return data_;}

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool StringKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        data_ = parser.argsv(startArg);
        set_ = true;

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool StringKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_);
}
