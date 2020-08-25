/*
    *** Keyword - Broadening Function
    *** src/keywords/broadeningfunction.cpp
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

#include "keywords/broadeningfunction.h"
#include "base/lineparser.h"

BroadeningFunctionKeyword::BroadeningFunctionKeyword(BroadeningFunction value)
    : KeywordData<BroadeningFunction>(KeywordBase::BroadeningFunctionData, value)
{
}

BroadeningFunctionKeyword::~BroadeningFunctionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BroadeningFunctionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int BroadeningFunctionKeyword::maxArguments() const { return MAXBROADENINGFUNCTIONPARAMS; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool BroadeningFunctionKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    auto result = data_.set(parser, startArg);
    if (result)
        set_ = true;

    return result;
}

// Write keyword data to specified LineParser
bool BroadeningFunctionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    std::string params;
    for (int n = 0; n < BroadeningFunction::nFunctionParameters(data_.function()); ++n)
        params += fmt::format("  {}", data_.parameter(n));
    return parser.writeLineF("{}{}  '{}'{}\n", prefix, keywordName, BroadeningFunction::functionType(data_.function()), params);
}
