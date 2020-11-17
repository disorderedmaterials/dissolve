// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "keywords/data.h"
#include "templates/list.h"

// Forward Declarations
class ExpressionVariable;
class ProcedureNode;

// Keyword with ExpressionVariable List
class ExpressionVariableVectorKeyword : public KeywordData<std::vector<std::shared_ptr<ExpressionVariable>> &>
{
    public:
    ExpressionVariableVectorKeyword(ProcedureNode *parentNode, std::vector<std::shared_ptr<ExpressionVariable>> &variables,
                                    ExpressionValue::ValueType variableType);
    ~ExpressionVariableVectorKeyword();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Return parent ProcedureNode
    const ProcedureNode *parentNode() const;

    /*
     * Variable Type
     */
    private:
    // Assumed type for variables in the list
    ExpressionValue::ValueType variableType_;

    public:
    // Return assumed type for variables in the list
    ExpressionValue::ValueType variableType() const;

    /*
     * Data
     */
    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const;

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
     * Object Management
     */
    protected:
};
