// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/root.h"
#include "templates/optionalref.h"

// Forward Declarations
class ExpressionVariable;

// Mathematical Expression
class Expression
{
    public:
    Expression(std::string_view expressionText = "");
    ~Expression();
    Expression(const Expression &source);
    void operator=(const Expression &source);

    /*
     * Data
     */
    private:
    // Original generating string
    std::string expressionString_;
    // Root node for the expression
    std::shared_ptr<ExpressionNode> rootNode_;

    public:
    // Clear data
    void clear();
    // Return whether current expression is valid (contains at least one node)
    bool isValid() const;
    // Create expression from supplied string, with optional external variables
    bool
    create(std::string_view expressionString,
           OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> externalVariables = std::nullopt);
    // Return original generating string
    std::string_view expressionString() const;
    // Return root node for the expression
    std::shared_ptr<ExpressionNode> rootNode();

    /*
     * Execution
     */
    public:
    // Evaluate the expression
    std::optional<ExpressionValue> evaluate() const;
    // Execute and return as integer
    int asInteger() const;
    // Execute and return as double
    double asDouble() const;
};
