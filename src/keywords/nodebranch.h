// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class NodeValue;
class SequenceProcedureNode;

// Keyword managing node sequence
class NodeBranchKeyword : public KeywordBase
{
    public:
    NodeBranchKeyword(std::shared_ptr<SequenceProcedureNode> &data, ProcedureNode *parentNode,
                      ProcedureNode::NodeContext branchContext);
    ~NodeBranchKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::shared_ptr<SequenceProcedureNode> &data_;
    // Parent ProcedureNode
    NodeRef parentNode_;
    // Context for the target branch
    ProcedureNode::NodeContext branchContext_;

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
};
