// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Forward Declarations
class Expression;
class ProcedureNode;

// Keyword with Expression
class ExpressionKeyword : public KeywordData<Expression &>
{
    public:
    ExpressionKeyword(Expression &expression);
    ~ExpressionKeyword();

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);

    /*
     * Set
     */
    public:
    // Set the value from supplied expression text
    bool setValue(std::string_view expressionText);

    /*
     * Conversion
     */
    public:
    // Return value (as int)
    int asInt();
    // Return value (as double)
    double asDouble();

    /*
     * Object Management
     */
    protected:
};
