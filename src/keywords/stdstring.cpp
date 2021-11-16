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
const std::string &StringKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int StringKeyword::minArguments() const { return 1; }

// Return minimum number of arguments accepted
int StringKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool StringKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        data_ = parser.argsv(startArg);
        set_ = true;

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool StringKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_);
}
