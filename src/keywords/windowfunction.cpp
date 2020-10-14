// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/windowfunction.h"
#include "base/lineparser.h"

WindowFunctionKeyword::WindowFunctionKeyword(WindowFunction value)
    : KeywordData<WindowFunction>(KeywordBase::WindowFunctionData, value)
{
}

WindowFunctionKeyword::~WindowFunctionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int WindowFunctionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int WindowFunctionKeyword::maxArguments() const { return MAXWINDOWFUNCTIONPARAMS; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool WindowFunctionKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    auto result = data_.set(parser, startArg);
    if (result)
        set_ = true;

    return result;
}

// Write keyword data to specified LineParser
bool WindowFunctionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    std::string params;
    for (int n = 0; n < WindowFunction::nFunctionParameters(data_.function()); ++n)
        params += fmt::format("  {}", data_.parameter(n));
    return parser.writeLineF("{}{}  '{}'{}\n", prefix, keywordName, WindowFunction::functionType(data_.function()), params);
}
