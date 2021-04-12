// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/broadeningfunction.h"
#include "base/lineparser.h"

BroadeningFunctionKeyword::BroadeningFunctionKeyword(BroadeningFunction value)
    : KeywordData<BroadeningFunction>(KeywordBase::BroadeningFunctionData, value)
{
}

BroadeningFunctionKeyword::~BroadeningFunctionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BroadeningFunctionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int BroadeningFunctionKeyword::maxArguments() const { return MAXBROADENINGFUNCTIONPARAMS; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool BroadeningFunctionKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    auto result = data_.set(parser, startArg);
    if (result)
        set_ = true;

    return result;
}

// Write keyword data to specified LineParser
bool BroadeningFunctionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    std::string params;
    for (auto n = 0; n < BroadeningFunction::nFunctionParameters(data_.function()); ++n)
        params += fmt::format("  {}", data_.parameter(n));
    return parser.writeLineF("{}{}  '{}'{}\n", prefix, keywordName, BroadeningFunction::functionType(data_.function()), params);
}
