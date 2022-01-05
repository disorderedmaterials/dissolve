// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/expression.h"
#include "base/lineparser.h"
#include "expression/expression.h"

ExpressionKeyword::ExpressionKeyword(Expression &data, const std::vector<std::shared_ptr<ExpressionVariable>> &variables)
    : KeywordBase(typeid(this)), data_(data), variables_(variables)
{
}

/*
 * Data
 */

// Return reference to data
const Expression &ExpressionKeyword::data() const { return data_; }

// Set data
bool ExpressionKeyword::setData(std::string_view expressionText)
{
    if (!data_.create(expressionText, variables_))
        return false;

    return true;
}

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
