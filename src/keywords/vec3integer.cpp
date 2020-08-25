/*
    *** Keyword - Integer Triplet
    *** src/keywords/vec3integer.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/vec3integer.h"
#include "base/lineparser.h"

Vec3IntegerKeyword::Vec3IntegerKeyword(Vec3<int> value, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<int>>(KeywordBase::Vec3IntegerData, value)
{
    labelType_ = labelType;
    minimumLimit_ = false;
    maximumLimit_ = false;
}

Vec3IntegerKeyword::Vec3IntegerKeyword(Vec3<int> value, Vec3<int> minValue, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<int>>(KeywordBase::Vec3IntegerData, value)
{
    labelType_ = labelType;
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

Vec3IntegerKeyword::Vec3IntegerKeyword(Vec3<int> value, Vec3<int> minValue, Vec3<int> maxValue, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<int>>(KeywordBase::Vec3IntegerData, value)
{
    labelType_ = labelType;
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

Vec3IntegerKeyword::~Vec3IntegerKeyword() {}

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType Vec3IntegerKeyword::labelType() const { return labelType_; }

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set for supplied index
bool Vec3IntegerKeyword::hasValidationMin(int index) { return minimumLimit_[index]; }

// Return validation minimum limit for supplied index
int Vec3IntegerKeyword::validationMin(int index) { return min_[index]; }

// Return whether a maximum validation limit has been set for supplied index
bool Vec3IntegerKeyword::hasValidationMax(int index) { return maximumLimit_[index]; }

// Return validation maximum limit for supplied index
int Vec3IntegerKeyword::validationMax(int index) { return max_[index]; }

// Validate supplied value
bool Vec3IntegerKeyword::isValid(Vec3<int> value)
{
    if (!isValid(0, value.x))
        return false;
    if (!isValid(1, value.y))
        return false;
    if (!isValid(2, value.z))
        return false;

    return true;
}

// Validate supplied value
bool Vec3IntegerKeyword::isValid(int index, int value)
{
    // Check minimum limit
    if (minimumLimit_[index])
    {
        if (value < min_[index])
            return false;
    }

    // Check maximum limit
    if (maximumLimit_[index])
    {
        if (value > max_[index])
            return false;
    }

    return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3IntegerKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
int Vec3IntegerKeyword::maxArguments() const { return 3; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool Vec3IntegerKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg + 2))
    {
        // Check individual components of the vector
        for (int n = 0; n < 3; ++n)
        {
            if (!isValid(n, parser.argi(startArg + n)))
            {
                if (minimumLimit_[n] && maximumLimit_[n])
                    Messenger::error("Value {} is out of range for keyword '{}'. Valid range is {} <= n <= {}.\n", data_[n],
                                     name(), min_[n], max_[n]);
                else if (minimumLimit_[n])
                    Messenger::error("Value {} is out of range for keyword '{}'. Valid range is {} <= n.\n", data_[n], name(),
                                     min_[n]);
                else
                    Messenger::error("Value {} is out of range for keyword '{}'. Valid range is n <= {}.\n", data_[n], name(),
                                     max_[n]);

                return false;
            }
        }

        if (!setData(parser.arg3i(startArg)))
            return false;

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool Vec3IntegerKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  {}  {}  {}\n", prefix, keywordName, data_.x, data_.y, data_.z);
}

/*
 * Conversion
 */

// Return value (as Vec3<int>)
Vec3<int> Vec3IntegerKeyword::asVec3Int() { return data_; }

// Return value (as Vec3<int>)
Vec3<double> Vec3IntegerKeyword::asVec3Double() { return Vec3<double>(data_.x, data_.y, data_.z); }
