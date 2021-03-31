// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/stdstring.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

StringKeyword::StringKeyword(std::string_view value) : KeywordData<std::string>(KeywordBase::StringData, std::string(value)) {}

StringKeyword::~StringKeyword() {}

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
        if (!setData(std::string(parser.argsv(startArg))))
            return false;

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool StringKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool StringKeyword::asBool() { return DissolveSys::stob(data_); }

// Return value (as int)
int StringKeyword::asInt() { return std::stoi(data_); }

// Return value (as double)
double StringKeyword::asDouble() { return std::stof(data_); }

// Return value (as string)
std::string StringKeyword::asString() { return data_; }
