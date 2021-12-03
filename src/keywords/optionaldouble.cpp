// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/optionaldouble.h"
#include "base/lineparser.h"

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
        if (maximumLimit_ && value > maximumLimit_)
            return false;
        else if (value.value() <= minimumLimit_)
            value = std::nullopt;
    }

    data_ = value;
    set_ = data_.has_value();

    return true;
}

// Return data
std::optional<double> OptionalDoubleKeyword::data() const { return data_; }

// Return validation minimum limit
double OptionalDoubleKeyword::validationMin() const { return minimumLimit_; }

// Return validation maximum limit
std::optional<double> OptionalDoubleKeyword::validationMax() const { return maximumLimit_; }

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
    if (parser.hasArg(startArg))
    {
        auto x = parser.argd(startArg);
        if (!setData(x))
        {
            if (maximumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n.\n", x, maximumLimit_.value());

            return false;
        }

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool OptionalDoubleKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_.has_value())
        return true;

    return parser.writeLineF("{}{}  {:12.5e}\n", prefix, keywordName, data_.value());
}
