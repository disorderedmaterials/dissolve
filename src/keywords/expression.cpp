// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    if (!data_.create(expressionText))
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
