// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/nodeValue.h"
#include "keywords/base.h"

// Forward Declarations
class NodeValue;
class GeneratorNode;

// Keyword managing NodeValue
class NodeValueKeyword : public KeywordBase
{
    public:
    NodeValueKeyword(NodeValue &value, GeneratorNode *parentNode);
    ~NodeValueKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    NodeValue &data_;
    // Initial value
    const NodeValue default_;
    // Parent GeneratorNode
    GeneratorNode *parentNode_;

    public:
    // Return reference to data
    NodeValue &data();
    const NodeValue &data() const;
    // Set the value from supplied expression text
    bool setData(std::string_view expressionText);
    // Set the value from NodeValue
    bool setData(const NodeValue &nodeValue);

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &cordeData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};
