// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/range.h"
#include "base/lineparser.h"

RangeKeyword::RangeKeyword(Range value, Vec3Labels::LabelType labelType) : KeywordData<Range>(KeywordBase::RangeData, value)
{
    labelType_ = labelType;
}

RangeKeyword::~RangeKeyword() {}

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType RangeKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int RangeKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int RangeKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool RangeKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg + 1))
    {
        setData(Range(parser.argd(startArg), parser.argd(startArg + 1)));

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool RangeKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.minimum(), data_.maximum());
}
