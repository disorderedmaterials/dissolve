// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/double.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

DoubleKeyword::DoubleKeyword(double value) : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = false;
    maximumLimit_ = false;
}

DoubleKeyword::DoubleKeyword(double value, double minValue) : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

DoubleKeyword::DoubleKeyword(double value, double minValue, double maxValue)
    : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

DoubleKeyword::~DoubleKeyword() {}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool DoubleKeyword::hasValidationMin() { return minimumLimit_; }

// Return validation minimum limit
double DoubleKeyword::validationMin() { return min_; }

// Return whether a maximum validation limit has been set
bool DoubleKeyword::hasValidationMax() { return maximumLimit_; }

// Return validation maximum limit
double DoubleKeyword::validationMax() { return max_; }

// Validate supplied value
bool DoubleKeyword::isValid(double value)
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
int DoubleKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int DoubleKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool DoubleKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        if (!setData(parser.argd(startArg)))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n <= {}.\n", parser.argd(startArg),
                                 min_, max_);
            else if (minimumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n.\n", parser.argd(startArg),
                                 min_);
            else
                Messenger::error("Value {} is out of range for keyword. Valid range is n <= {}.\n", parser.argd(startArg),
                                 max_);

            return false;
        }

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool DoubleKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {:12.5e}\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool DoubleKeyword::asBool() { return data_; }

// Return value (as int)
int DoubleKeyword::asInt() { return data_; }

// Return value (as double)
double DoubleKeyword::asDouble() { return data_; }

// Return value (as string)
std::string DoubleKeyword::asString() { return fmt::format("{}", data_); }
