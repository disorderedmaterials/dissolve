// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/nodeNumber.h"

// MultiplyNode Node
class MultiplyNode : public Node
{
    public:
    MultiplyNode();
    ~MultiplyNode() override = default;

    public:
    std::string_view name() const override;
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
    T product_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Node::ProcessResult process(ModuleContext &moduleContext);
};
