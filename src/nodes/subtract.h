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
    std::string_view name() const override { return "Subtract"; }
    std::string_view summary() const override { return "Performs the subtraction A - B"; }

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
