// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/integer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

IntegerKeyword::IntegerKeyword(int value) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = false;
    maximumLimit_ = false;
}

IntegerKeyword::IntegerKeyword(int value, int minValue) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

IntegerKeyword::IntegerKeyword(int value, int minValue, int maxValue) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

IntegerKeyword::~IntegerKeyword() {}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool IntegerKeyword::hasValidationMin() { return minimumLimit_; }

// Return validation minimum limit
int IntegerKeyword::validationMin() { return min_; }

// Return whether a maximum validation limit has been set
bool IntegerKeyword::hasValidationMax() { return maximumLimit_; }

// Return validation maximum limit
int IntegerKeyword::validationMax() { return max_; }

// Validate supplied value
bool IntegerKeyword::isValid(int value)
{
    // Check minimum limit
    if (minimumLimit_)
    {
        if (value < min_)
            return false;
    }

    // Check maximum limit
    if (maximumLimit_)
    {
        if (value > max_)
            return false;
    }

    return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IntegerKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int IntegerKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool IntegerKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        if (!setData(parser.argi(startArg)))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n <= {}.\n", parser.argi(startArg),
                                 min_, max_);
            else if (minimumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n.\n", parser.argi(startArg),
                                 min_);
            else
                Messenger::error("Value {} is out of range for keyword. Valid range is n <= {}.\n", parser.argi(startArg),
                                 max_);

            return false;
        }

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool IntegerKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool IntegerKeyword::asBool() { return data_; }

// Return value (as int)
int IntegerKeyword::asInt() { return data_; }

// Return value (as double)
double IntegerKeyword::asDouble() { return data_ * 1.0; }

// Return value (as string)
std::string IntegerKeyword::asString() { return fmt::format("{}", data_); }
