// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "keywords/vec3Integer.h"
#include "base/lineParser.h"

Vec3IntegerKeyword::Vec3IntegerKeyword(Vector3i &data, std::optional<Vector3i> minValue, std::optional<Vector3i> maxValue,
                                       Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), default_(data), minimumLimit_(minValue), maximumLimit_(maxValue),
      labelType_(labelType)
{
}

/*
 * Data
 */

// Set data
bool Vec3IntegerKeyword::setData(Vector3i value)
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
const Vector3i &Vec3IntegerKeyword::data() const { return data_; }

// Return minimum limit
std::optional<Vector3i> Vec3IntegerKeyword::minimumLimit() const { return minimumLimit_; }

// Return maximum limit
std::optional<Vector3i> Vec3IntegerKeyword::maximumLimit() const { return maximumLimit_; }

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType Vec3IntegerKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3IntegerKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
std::optional<int> Vec3IntegerKeyword::maxArguments() const { return 3; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Vec3IntegerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg + 2))
    {
        auto v = parser.arg3i(startArg);
        if (!setData(v))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value [{},{},{}] is out of range. Valid range is "
                                 "[{},{},{}] <= [x,y,z] "
                                 "<= [{},{},{}].\n",
                                 v.x, v.y, v.z, minimumLimit_.value().x, minimumLimit_.value().y, minimumLimit_.value().z,
                                 maximumLimit_.value().x, maximumLimit_.value().y, maximumLimit_.value().z);
            else if (minimumLimit_)
                Messenger::error("Value [{},{},{}] is out of range. Valid range is "
                                 "[{},{},{}] <= [x,y,z].\n",
                                 v.x, v.y, v.z, minimumLimit_.value().x, minimumLimit_.value().y, minimumLimit_.value().z);
            else
                Messenger::error("Value [{},{},{}] is out of range. Valid range is [x,y,z] "
                                 "<= [{},{},{}].\n",
                                 v.x, v.y, v.z, maximumLimit_.value().x, maximumLimit_.value().y, maximumLimit_.value().z);

            return false;
        }

        return true;
    }
    return false;
}

// Serialise data to specified LineParser
bool Vec3IntegerKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}  {}  {}\n", prefix, keywordName, data_.x, data_.y, data_.z);
}

// Express as a serialisable value
void Vec3IntegerKeyword::serialise(std::string name, SerialisedValue &target) const { target[name] = data_; }

// Read values from a serialisable value
void Vec3IntegerKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_.deserialise(node); }

// Has not changed from initial value
bool Vec3IntegerKeyword::isDefault() const { return data_ == default_; }
