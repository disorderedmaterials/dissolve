// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class NodeValue;
class SequenceProcedureNode;

// Keyword with node sequence
class NodeBranchKeyword : public KeywordBase
{
    public:
    NodeBranchKeyword(SequenceProcedureNode *&data, NodeRef parentNode, ProcedureNode::NodeContext branchContext);
    ~NodeBranchKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    SequenceProcedureNode *&data_;
    // Parent ProcedureNode
    NodeRef parentNode_;
    // Context for the target branch
    ProcedureNode::NodeContext branchContext_;

    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override;

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
};
