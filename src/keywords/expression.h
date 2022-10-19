// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Expression;
class ExpressionVariable;

// Keyword managing Expression
class ExpressionKeyword : public KeywordBase
{
    public:
    ExpressionKeyword(Expression &data, const std::vector<std::shared_ptr<ExpressionVariable>> &variables);
    ~ExpressionKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Expression &data_;
    // Vector of variables available to the expression
    const std::vector<std::shared_ptr<ExpressionVariable>> &variables_;

    public:
    // Return reference to data
    const Expression &data() const;
    // Set data
    bool setData(std::string_view expressionText);

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node) override;
};
