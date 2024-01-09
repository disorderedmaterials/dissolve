// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/optionalInt.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/base.h"

OptionalIntegerKeyword::OptionalIntegerKeyword(std::optional<int> &data, int minValue, std::optional<int> maxValue,
                                               int valueDelta, std::string_view textWhenNull)
    : KeywordBase(typeid(this)), data_(data), minimumLimit_{minValue}, maximumLimit_{maxValue},
      valueDelta_(valueDelta), textWhenNull_{textWhenNull}
{
}

/*
 * Data
 */

// Set data
bool OptionalIntegerKeyword::setData(std::optional<int> value)
{
    // Check limits if a value was supplied
    if (value)
    {
        if (maximumLimit_ && value.value() > maximumLimit_)
            return false;
        else if (value.value() <= minimumLimit_)
            value = std::nullopt;
    }

    data_ = value;
    set_ = true;

    return true;
}

// Return data
std::optional<int> OptionalIntegerKeyword::data() const { return data_; }

// Return minimum limit
int OptionalIntegerKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<int> OptionalIntegerKeyword::maximumLimit() const { return maximumLimit_; }

// Return step size for widget
int OptionalIntegerKeyword::valueDelta() const { return valueDelta_; }

// Return text to display in widget  when value is null
std::string OptionalIntegerKeyword::textWhenNull() const { return textWhenNull_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool OptionalIntegerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    try
    {
        if (!parser.hasArg(startArg))
            return false;

        auto x = parser.argi(startArg);
        if (!setData(x))
        {
            if (maximumLimit_)
                return Messenger::error("Value {} is out of range for keyword '{}'. Valid range is {} <= n.\n", x, name(),
                                        maximumLimit_.value());
        }
    }
    catch (...)
    {
        // Check explicitly for null text
        if (DissolveSys::sameString(parser.argsv(startArg), textWhenNull_))
            setData(std::nullopt);
        else
            return Messenger::error("Input {} is invalid for keyword '{}'.\n", parser.argsv(startArg), name());
    }
    return true;
}

// Serialise data to specified LineParser
bool OptionalIntegerKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!set_)
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_.value_or(minimumLimit_));
}

// Express as a serialisable value
SerialisedValue OptionalIntegerKeyword::serialise() const { return data_.value_or(minimumLimit_); }

// Read values from a serialisable value
void OptionalIntegerKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    setData(toml::get<int>(node));
}
