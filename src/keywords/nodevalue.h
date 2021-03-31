// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword with NodeValue
class NodeValueKeyword : public KeywordData<NodeValue>
{
    public:
    NodeValueKeyword(ProcedureNode *parentNode, NodeValue value);
    ~NodeValueKeyword();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;

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
