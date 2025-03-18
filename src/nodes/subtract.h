// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/nodeNumber.h"

// SubractNode Node
class SubtractNode : public Node
{
    public:
    SubtractNode();
    ~SubtractNode() override = default;

    public:
    // Return short name of the node
    std::string_view name() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Factor A
    T a_;
    // Factor B
    T b_;
    // Product of A and B
    T result_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Node::ProcessResult process(ModuleContext &moduleContext);
};
