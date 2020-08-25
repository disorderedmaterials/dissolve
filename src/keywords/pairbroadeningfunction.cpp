/*
    *** Keyword - Pair Broadening Function
    *** src/keywords/pairbroadeningfunction.cpp
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

#include "keywords/pairbroadeningfunction.h"
#include "base/lineparser.h"

PairBroadeningFunctionKeyword::PairBroadeningFunctionKeyword(PairBroadeningFunction value)
    : KeywordData<PairBroadeningFunction>(KeywordBase::PairBroadeningFunctionData, value)
{
}

PairBroadeningFunctionKeyword::~PairBroadeningFunctionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int PairBroadeningFunctionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int PairBroadeningFunctionKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool PairBroadeningFunctionKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (data_.readAsKeyword(parser, startArg, coreData))
        set_ = true;
    else
        return false;

    return true;
}

// Write keyword data to specified LineParser
bool PairBroadeningFunctionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    if (!parser.writeLineF("{}{}", prefix, name()))
        return false;

    return data_.writeAsKeyword(parser, prefix, true);
}

/*
 * Validation
 */

// Validate supplied value
bool PairBroadeningFunctionKeyword::isValid(PairBroadeningFunction value) { return true; }
