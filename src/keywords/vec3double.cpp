/*
    *** Keyword - Double Triplet
    *** src/keywords/vec3double.cpp
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

#include "keywords/vec3double.h"
#include "base/lineparser.h"

Vec3DoubleKeyword::Vec3DoubleKeyword(Vec3<double> value, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<double>>(KeywordBase::Vec3DoubleData, value)
{
    labelType_ = labelType;
    minimumLimit_ = false;
    maximumLimit_ = false;
}

Vec3DoubleKeyword::Vec3DoubleKeyword(Vec3<double> value, Vec3<double> minValue, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<double>>(KeywordBase::Vec3DoubleData, value)
{
    labelType_ = labelType;
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

Vec3DoubleKeyword::Vec3DoubleKeyword(Vec3<double> value, Vec3<double> minValue, Vec3<double> maxValue,
                                     Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<double>>(KeywordBase::Vec3DoubleData, value)
{
    labelType_ = labelType;
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

Vec3DoubleKeyword::~Vec3DoubleKeyword() {}

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType Vec3DoubleKeyword::labelType() const { return labelType_; }

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set for supplied index
bool Vec3DoubleKeyword::hasValidationMin(int index) { return minimumLimit_[index]; }

// Return validation minimum limit for supplied index
double Vec3DoubleKeyword::validationMin(int index) { return min_[index]; }

// Return whether a maximum validation limit has been set for supplied index
bool Vec3DoubleKeyword::hasValidationMax(int index) { return maximumLimit_[index]; }

// Return validation maximum limit for supplied index
double Vec3DoubleKeyword::validationMax(int index) { return max_[index]; }

// Validate supplied value
bool Vec3DoubleKeyword::isValid(Vec3<double> value)
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
bool Vec3DoubleKeyword::isValid(int index, double value)
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
int Vec3DoubleKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
int Vec3DoubleKeyword::maxArguments() const { return 3; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool Vec3DoubleKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg + 2))
    {
        // Check individual components of the vector
        for (int n = 0; n < 3; ++n)
        {
            if (!isValid(n, parser.argd(startArg + n)))
            {
                if (minimumLimit_[n] && maximumLimit_[n])
                    Messenger::error("Value {:12.6e} is out of range for keyword '{}'. Valid range is  <= n "
                                     "<= {:12.6e}.\n",
                                     data_[n], name(), min_[n], max_[n]);
                else if (minimumLimit_[n])
                    Messenger::error("Value {:12.6e} is out of range for keyword '{}'. Valid range is {:12.6e} <= n.\n",
                                     data_[n], name(), min_[n]);
                else
                    Messenger::error("Value {:12.6e} is out of range for keyword '{}'. Valid range is n <= {:12.6e}.\n",
                                     data_[n], name(), max_[n]);

                return false;
            }
        }

        if (!setData(parser.arg3d(startArg)))
            return false;

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool Vec3DoubleKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.x, data_.y, data_.z);
}

/*
 * Conversion
 */

// Return value (as Vec3<int>)
Vec3<int> Vec3DoubleKeyword::asVec3Int() { return Vec3<int>(data_.x, data_.y, data_.z); }

// Return value (as Vec3<double>)
Vec3<double> Vec3DoubleKeyword::asVec3Double() { return data_; }
