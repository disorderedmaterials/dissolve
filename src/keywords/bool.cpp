// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

BoolKeyword::BoolKeyword(bool &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Set data
bool BoolKeyword::setData(bool value)
{
    data_ = value;
    set_ = true;

    return true;
}

// Return data
bool &BoolKeyword::data() { return data_; }
const bool &BoolKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BoolKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int BoolKeyword::maxArguments() const { return 1; }

// Deserialise from supplied LineParser, starting at given argument offset
bool BoolKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        data_ = parser.argb(startArg);
        set_ = true;

        return true;
    }
    return false;
}

// Serialise data to specified LineParser
bool BoolKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, DissolveSys::btoa(data_));
}
