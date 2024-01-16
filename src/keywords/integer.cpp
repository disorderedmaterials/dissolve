// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/integer.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

IntegerKeyword::IntegerKeyword(int &data, std::optional<int> minValue, std::optional<int> maxValue)
    : KeywordBase(typeid(this)), data_(data), default_(data), minimumLimit_(minValue), maximumLimit_(maxValue)
{
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

// Return minimum limit
std::optional<int> IntegerKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<int> IntegerKeyword::maximumLimit() const { return maximumLimit_; }

/*
 * Arguments
 */

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
    if (!set_)
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_);
}

// Express as a serialisable value
SerialisedValue IntegerKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void IntegerKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_ = node.as_integer(); }

// Has not changed from initial value
bool IntegerKeyword::isDefault() const { return data_ == default_; }
