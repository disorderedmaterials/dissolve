// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class NodeValue;
class ProcedureNode;

// Keyword managing NodeValue
class NodeValueKeyword : public KeywordBase
{
    public:
    NodeValueKeyword(NodeValue &value, ProcedureNode *parentNode);
    ~NodeValueKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    NodeValue &data_;
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Return reference to data
    NodeValue &data();
    const NodeValue &data() const;
    // Set the value from supplied expression text
    bool setData(std::string_view expressionText);

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
