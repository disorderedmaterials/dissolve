// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/vec3Double.h"
#include "base/lineParser.h"

Vec3DoubleKeyword::Vec3DoubleKeyword(Vec3<double> &data, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), default_(data), labelType_(labelType)
{
}

Vec3DoubleKeyword::Vec3DoubleKeyword(Vec3<double> &data, std::optional<Vec3<double>> minValue,
                                     std::optional<Vec3<double>> maxValue, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), minimumLimit_(minValue), maximumLimit_(maxValue), labelType_(labelType)
{
}

/*
 * Data
 */

// Set data
bool Vec3DoubleKeyword::setData(Vec3<double> value)
{
    if (minimumLimit_ &&
        (value.x < minimumLimit_.value().x || value.y < minimumLimit_.value().y || value.z < minimumLimit_.value().z))
        return false;
    if (maximumLimit_ &&
        (value.x > maximumLimit_.value().x || value.y > maximumLimit_.value().y || value.z > maximumLimit_.value().z))
        return false;

    data_ = value;

    return true;
}

// Return data
const Vec3<double> &Vec3DoubleKeyword::data() const { return data_; }

// Return minimum limit
std::optional<Vec3<double>> Vec3DoubleKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<Vec3<double>> Vec3DoubleKeyword::maximumLimit() const { return maximumLimit_; }

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType Vec3DoubleKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3DoubleKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
std::optional<int> Vec3DoubleKeyword::maxArguments() const { return 3; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Vec3DoubleKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg + 2))
    {
        auto v = parser.arg3d(startArg);
        if (!setData(v))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value [{:12.6e},{:12.6e},{:12.6e}] is out of range. Valid range is "
                                 "[{:12.6e},{:12.6e},{:12.6e}] <= [x,y,z] "
                                 "<= [{:12.6e},{:12.6e},{:12.6e}].\n",
                                 v.x, v.y, v.z, minimumLimit_.value().x, minimumLimit_.value().y, minimumLimit_.value().z,
                                 maximumLimit_.value().x, maximumLimit_.value().y, maximumLimit_.value().z);
            else if (minimumLimit_)
                Messenger::error("Value [{:12.6e},{:12.6e},{:12.6e}] is out of range. Valid range is "
                                 "[{:12.6e},{:12.6e},{:12.6e}] <= [x,y,z].\n",
                                 v.x, v.y, v.z, minimumLimit_.value().x, minimumLimit_.value().y, minimumLimit_.value().z);
            else
                Messenger::error("Value [{:12.6e},{:12.6e},{:12.6e}] is out of range. Valid range is [x,y,z] "
                                 "<= [{:12.6e},{:12.6e},{:12.6e}].\n",
                                 v.x, v.y, v.z, maximumLimit_.value().x, maximumLimit_.value().y, maximumLimit_.value().z);

            return false;
        }

        return true;
    }
    return false;
}

// Serialise data to specified LineParser
bool Vec3DoubleKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.x, data_.y, data_.z);
}

// Has not changed from initial value
bool Vec3DoubleKeyword::isDefault() const { return data_ == default_; }

// Express as a serialisable value
SerialisedValue Vec3DoubleKeyword::serialise() const { return data_.serialise(); }

// Read values from a serialisable value
void Vec3DoubleKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_.deserialise(node); }
