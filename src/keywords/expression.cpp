// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/expression.h"
#include "base/lineparser.h"
#include "expression/expression.h"

ExpressionKeyword::ExpressionKeyword(Expression &data, const std::vector<std::shared_ptr<ExpressionVariable>> &variables)
    : KeywordBase(typeid(this), KeywordBase::ExpressionData), data_(data), variables_(variables)
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

    set_ = true;

    return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ExpressionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ExpressionKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ExpressionKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    return setData(parser.argsv(startArg));
}

// Write keyword data to specified LineParser
bool ExpressionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.expressionString()))
        return false;

    return true;
}
