// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/pairbroadeningfunction.h"
#include "base/lineparser.h"

PairBroadeningFunctionKeyword::PairBroadeningFunctionKeyword(PairBroadeningFunction value)
    : KeywordData<PairBroadeningFunction>(KeywordBase::PairBroadeningFunctionData, value)
{
}

PairBroadeningFunctionKeyword::~PairBroadeningFunctionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int PairBroadeningFunctionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int PairBroadeningFunctionKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool PairBroadeningFunctionKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (data_.readAsKeyword(parser, startArg, coreData))
        set_ = true;
    else
        return false;

    return true;
}

// Write keyword data to specified LineParser
bool PairBroadeningFunctionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}", prefix, name()))
        return false;

    return data_.writeAsKeyword(parser, prefix, true);
}

/*
 * Validation
 */

// Validate supplied value
bool PairBroadeningFunctionKeyword::isValid(PairBroadeningFunction value) { return true; }
