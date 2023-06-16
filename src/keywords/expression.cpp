// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/expression.h"
#include "base/lineparser.h"
#include "expression/expression.h"

ExpressionKeyword::ExpressionKeyword(Expression &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
const Expression &ExpressionKeyword::data() const { return data_; }

// Set data
bool ExpressionKeyword::setData(std::string_view expressionText) { return data_.create(expressionText); }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool ExpressionKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    return setData(parser.argsv(startArg));
}

// Serialise data to specified LineParser
bool ExpressionKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.expressionString()))
        return false;

    return true;
}
