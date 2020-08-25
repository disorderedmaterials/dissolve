/*
    *** Keyword - Double
    *** src/keywords/double.cpp
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

#include "keywords/double.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

DoubleKeyword::DoubleKeyword(double value) : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = false;
    maximumLimit_ = false;
}

DoubleKeyword::DoubleKeyword(double value, double minValue) : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

DoubleKeyword::DoubleKeyword(double value, double minValue, double maxValue)
    : KeywordData<double>(KeywordBase::DoubleData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

DoubleKeyword::~DoubleKeyword() {}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool DoubleKeyword::hasValidationMin() { return minimumLimit_; }

// Return validation minimum limit
double DoubleKeyword::validationMin() { return min_; }

// Return whether a maximum validation limit has been set
bool DoubleKeyword::hasValidationMax() { return maximumLimit_; }

// Return validation maximum limit
double DoubleKeyword::validationMax() { return max_; }

// Validate supplied value
bool DoubleKeyword::isValid(double value)
{
    // Check minimum limit
    if (minimumLimit_)
    {
        if (value < min_)
            return false;
    }

    // Check maximum limit
    if (maximumLimit_)
    {
        if (value > max_)
            return false;
    }

    return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DoubleKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int DoubleKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool DoubleKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        if (!setData(parser.argd(startArg)))
        {
            if (minimumLimit_ && maximumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n <= {}.\n", data_, min_, max_);
            else if (minimumLimit_)
                Messenger::error("Value {} is out of range for keyword. Valid range is {} <= n.\n", data_, min_);
            else
                Messenger::error("Value {} is out of range for keyword. Valid range is n <= {}.\n", data_, max_);

            return false;
        }

        return true;
    }
    return false;
}

// Write keyword data to specified LineParser
bool DoubleKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  {:12.5e}\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool DoubleKeyword::asBool() { return data_; }

// Return value (as int)
int DoubleKeyword::asInt() { return data_; }

// Return value (as double)
double DoubleKeyword::asDouble() { return data_; }

// Return value (as string)
std::string DoubleKeyword::asString() { return fmt::format("{}", data_); }
