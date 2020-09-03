/*
    *** Keyword - Integer
    *** src/keywords/integer.cpp
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

#include "keywords/integer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

IntegerKeyword::IntegerKeyword(int value) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = false;
    maximumLimit_ = false;
}

IntegerKeyword::IntegerKeyword(int value, int minValue) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = false;
}

IntegerKeyword::IntegerKeyword(int value, int minValue, int maxValue) : KeywordData<int>(KeywordBase::IntegerData, value)
{
    minimumLimit_ = true;
    min_ = minValue;
    maximumLimit_ = true;
    max_ = maxValue;
}

IntegerKeyword::~IntegerKeyword() {}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool IntegerKeyword::hasValidationMin() { return minimumLimit_; }

// Return validation minimum limit
int IntegerKeyword::validationMin() { return min_; }

// Return whether a maximum validation limit has been set
bool IntegerKeyword::hasValidationMax() { return maximumLimit_; }

// Return validation maximum limit
int IntegerKeyword::validationMax() { return max_; }

// Validate supplied value
bool IntegerKeyword::isValid(int value)
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
int IntegerKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int IntegerKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool IntegerKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        if (!setData(parser.argi(startArg)))
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
bool IntegerKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool IntegerKeyword::asBool() { return data_; }

// Return value (as int)
int IntegerKeyword::asInt() { return data_; }

// Return value (as double)
double IntegerKeyword::asDouble() { return data_ * 1.0; }

// Return value (as string)
std::string IntegerKeyword::asString() { return fmt::format("{}", data_); }
