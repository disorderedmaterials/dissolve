// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/integer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

IntegerKeyword::IntegerKeyword(int &data, std::optional<int> minValue, std::optional<int> maxValue)
    : KeywordBase(typeid(this)), data_(data)
{
    minimumLimit_ = minValue;
    maximumLimit_ = maxValue;
}

/*
 * Data
 */

// Set data
bool IntegerKeyword::setData(int value)
{
    if ((minimumLimit_ && value < minimumLimit_.value()) || (maximumLimit_ && value > maximumLimit_))
        return false;

    data_ = value;
    set_ = true;

    return true;
}

// Return data
int IntegerKeyword::data() const { return data_; }

// Return validation minimum limit
std::optional<int> IntegerKeyword::validationMin() { return minimumLimit_; }

// Return validation maximum limit
std::optional<int> IntegerKeyword::validationMax() { return maximumLimit_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IntegerKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int IntegerKeyword::maxArguments() const { return 1; }

// Deserialise from supplied LineParser, starting at given argument offset
bool IntegerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        auto x = parser.argi(startArg);
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

// Serialise data to specified LineParser
bool IntegerKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_);
}
