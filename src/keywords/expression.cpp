/*
    *** Keyword - Expression
    *** src/keywords/expression.cpp
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

#include "keywords/expression.h"
#include "base/lineparser.h"
#include "expression/expression.h"

ExpressionKeyword::ExpressionKeyword(Expression &expression)
    : KeywordData<Expression &>(KeywordData::ExpressionData, expression)
{
}

ExpressionKeyword::~ExpressionKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ExpressionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ExpressionKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ExpressionKeyword::read(LineParser &parser, int startArg, CoreData &coreData) { return setValue(parser.argsv(startArg)); }

// Write keyword data to specified LineParser
bool ExpressionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.expressionString()))
        return false;

    return true;
}

/*
 * Set
 */

// Set the value from supplied expression text
bool ExpressionKeyword::setValue(std::string_view expressionText)
{
    if (!data_.set(expressionText))
        return false;

    set_ = true;

    return true;
}

/*
 * Conversion
 */

// Return value (as int)
int ExpressionKeyword::asInt() { return data_.asInteger(); }

// Return value (as double)
double ExpressionKeyword::asDouble() { return data_.asDouble(); }

/*
 * Object Management
 */
