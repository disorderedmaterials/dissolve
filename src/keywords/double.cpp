// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/double.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

DoubleKeyword::DoubleKeyword(double &data, std::optional<double> minValue, std::optional<double> maxValue)
    : KeywordBase(typeid(this)), data_(data)
{
    minimumLimit_ = minValue;
    maximumLimit_ = maxValue;
}

/*
 * Data
 */

// Set data
bool DoubleKeyword::setData(double value)
{
    if ((minimumLimit_ && value < minimumLimit_.value()) || (maximumLimit_ && value > maximumLimit_))
        return false;

    data_ = value;
    set_ = true;

    return true;
}

// Return data
double DoubleKeyword::data() const { return data_; }

// Return validation minimum limit
std::optional<double> DoubleKeyword::validationMin() { return minimumLimit_; }

// Return validation maximum limit
std::optional<double> DoubleKeyword::validationMax() { return maximumLimit_; }

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
        auto x = parser.argd(startArg);
        if (!setData(x))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n <= {}.\n", x,
                                 minimumLimit_.value(), maximumLimit_.value());
            else if (minimumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n.\n", x, minimumLimit_.value());
            else
                Messenger::error("Value {} is out of range for keyword. Valid range is n <= {}.\n", x, maximumLimit_.value());

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
