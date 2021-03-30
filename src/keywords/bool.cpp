// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

BoolKeyword::BoolKeyword(bool value) : KeywordData<bool>(KeywordBase::BoolData, value) {}

BoolKeyword::~BoolKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BoolKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int BoolKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool BoolKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        setData(parser.argb(startArg));

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool BoolKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, DissolveSys::btoa(data_));
}

/*
 * Conversion
 */

// Return value (as bool)
bool BoolKeyword::asBool() { return data_; }

// Return value (as int)
int BoolKeyword::asInt() { return data_ ? 1 : 0; }

// Return value (as double)
double BoolKeyword::asDouble() { return data_ ? 1.0 : 0.0; }

// Return value (as string)
std::string BoolKeyword::asString() { return std::string(DissolveSys::btoa(data_)); }
