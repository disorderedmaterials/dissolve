// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/optionalDouble.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/base.h"

OptionalDoubleKeyword::OptionalDoubleKeyword(std::optional<double> &data, double minValue, std::optional<double> maxValue,
                                             double valueDelta, std::string_view textWhenNull)
    : KeywordBase(typeid(this)), data_(data), minimumLimit_{minValue}, maximumLimit_{maxValue},
      valueDelta_(valueDelta), textWhenNull_{textWhenNull}
{
}

/*
 * Data
 */

// Set data
bool OptionalDoubleKeyword::setData(std::optional<double> value)
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
std::optional<double> OptionalDoubleKeyword::data() const { return data_; }

// Return minimum limit
double OptionalDoubleKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<double> OptionalDoubleKeyword::maximumLimit() const { return maximumLimit_; }

// Return step size for widget
double OptionalDoubleKeyword::valueDelta() const { return valueDelta_; }

// Return text to display in widget  when value is null
std::string OptionalDoubleKeyword::textWhenNull() const { return textWhenNull_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool OptionalDoubleKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    try
    {
        if (!parser.hasArg(startArg))
            return false;

        auto x = parser.argd(startArg);
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
bool OptionalDoubleKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!set_)
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_.value_or(minimumLimit_));
}

// Express as a serialisable value
SerialisedValue OptionalDoubleKeyword::serialise() const { return data_.value_or(minimumLimit_); }

// Read values from a serialisable value
void OptionalDoubleKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    setData(toml::get<double>(node));
}

// Has not changed from initial value
bool OptionalDoubleKeyword::isDefault() const { return !set_ || !data_; }
