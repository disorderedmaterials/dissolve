// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "nodes/number.h"
#include "templates/vector3.h"

// DotProduct Node
class DotProductNode : public Node
{
    public:
    DotProductNode(Graph *parentGraph);
    ~DotProductNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Input vector 1
    Vec3<double> u_;
    // Input vector 2
    Vec3<double> v_;
    // Dot product value
    Number dotProduct_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
