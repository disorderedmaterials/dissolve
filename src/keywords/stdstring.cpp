/*
    *** Keyword - String
    *** src/keywords/stdstring.cpp
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

#include "keywords/stdstring.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

StringKeyword::StringKeyword(std::string_view value) : KeywordData<std::string>(KeywordBase::StringData, std::string(value)) {}

StringKeyword::~StringKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int StringKeyword::minArguments() const { return 1; }

// Return minimum number of arguments accepted
int StringKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool StringKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        if (!setData(std::string(parser.argsv(startArg))))
            return false;

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool StringKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool StringKeyword::asBool() { return DissolveSys::stob(data_); }

// Return value (as int)
int StringKeyword::asInt() { return std::stoi(data_); }

// Return value (as double)
double StringKeyword::asDouble() { return std::stof(data_); }

// Return value (as string)
std::string StringKeyword::asString() { return data_; }
