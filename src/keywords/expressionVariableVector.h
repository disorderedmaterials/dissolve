// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "keywords/base.h"

// Forward Declarations
class ExpressionVariable;
class GeneratorNode;

// Keyword managing vector of ExpressionVariable
class ExpressionVariableVectorKeyword : public KeywordBase
{
    public:
    ExpressionVariableVectorKeyword(std::vector<std::shared_ptr<ExpressionVariable>> &data, GeneratorNode *parentNode);
    ~ExpressionVariableVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to vector of data
    std::vector<std::shared_ptr<ExpressionVariable>> &data_;
    // Parent GeneratorNode
    GeneratorNode *parentNode_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Return reference to vector of data
    std::vector<std::shared_ptr<ExpressionVariable>> &data();
    const std::vector<std::shared_ptr<ExpressionVariable>> &data() const;
    // Return parent GeneratorNode
    const GeneratorNode *parentNode() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
