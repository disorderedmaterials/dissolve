// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/range.h"
#include "base/lineParser.h"

RangeKeyword::RangeKeyword(Range &data, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), default_(data), labelType_(labelType)
{
}

RangeKeyword::RangeKeyword(Range &data, std::optional<double> minimumLimit, std::optional<double> maximumLimit,
                           Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), minimumLimit_(minimumLimit), maximumLimit_(maximumLimit), labelType_(labelType)
{
}

/*
 * Data
 */

// Set data
bool RangeKeyword::setData(double rangeMin, double rangeMax)
{
    if ((minimumLimit_ && rangeMin < minimumLimit_.value()) || (maximumLimit_ && rangeMax > maximumLimit_.value()))
        return false;

    data_.set(rangeMin, rangeMax);

    return true;
}
bool RangeKeyword::setData(const Range &range) { return setData(range.minimum(), range.maximum()); }

// Set range minimum
bool RangeKeyword::setMinimum(double rangeMin)
{
    if ((minimumLimit_ && rangeMin < minimumLimit_.value()) || (maximumLimit_ && rangeMin > maximumLimit_) ||
        rangeMin > data_.maximum())
        return false;

    data_.setMinimum(rangeMin);

    return true;
}

// Set range maximum
bool RangeKeyword::setMaximum(double rangeMax)
{
    if ((minimumLimit_ && rangeMax < minimumLimit_.value()) || (maximumLimit_ && rangeMax > maximumLimit_) ||
        rangeMax < data_.minimum())
        return false;

    data_.setMaximum(rangeMax);

    return true;
}

// Return reference to data
const Range &RangeKeyword::data() const { return data_; }

// Return minimum limit
std::optional<double> RangeKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<double> RangeKeyword::maximumLimit() const { return maximumLimit_; }

// Label type to display in GUI
Vec3Labels::LabelType RangeKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int RangeKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> RangeKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool RangeKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg + 1))
    {
        auto rangeMin = parser.argd(startArg);
        auto rangeMax = parser.argd(startArg + 1);
        if (!setData(rangeMin, rangeMax))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Range {} - {} is invalid for keyword. Range limits are {} - {}.\n", rangeMin, rangeMax,
                                 minimumLimit_.value(), maximumLimit_.value());
            else if (minimumLimit_)
                Messenger::error("Range {} - {} is invalid for keyword. Range minimum limit is {}.\n", rangeMin, rangeMax,
                                 minimumLimit_.value());
            else
                Messenger::error("Range {} - {} is invalid for keyword. Range maximum limit is {}.\n", rangeMin, rangeMax,
                                 maximumLimit_.value());

            return false;
        }

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool RangeKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.minimum(), data_.maximum());
}

// Express as a serialisable value
SerialisedValue RangeKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void RangeKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_.deserialise(node); }

// Has not changed from initial value
bool RangeKeyword::isDefault() const { return data_ == default_; }
