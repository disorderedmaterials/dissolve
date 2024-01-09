// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/double.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

DoubleKeyword::DoubleKeyword(double &data, std::optional<double> minValue, std::optional<double> maxValue)
    : KeywordBase(typeid(this)), data_(data), default_(data), minimumLimit_(minValue), maximumLimit_(maxValue)
{
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

// Return minimum limit
std::optional<double> DoubleKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<double> DoubleKeyword::maximumLimit() const { return maximumLimit_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool DoubleKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
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

// Serialise data to specified LineParser
bool DoubleKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!set_)
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_);
}

// Express as a serialisable value
SerialisedValue DoubleKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void DoubleKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_ = toml::get<double>(node); }

// Has not changed from initial value
bool DoubleKeyword::isDefault() const { return data_ == default_; }
