// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"
#include "nodes/number.h"

// Vec3Decomposition Node
class Vec3DecompositionNode : public Node
{
    public:
    Vec3DecompositionNode(Graph *parentGraph);
    ~Vec3DecompositionNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Input vector
    Vector3 inputVector_;
    // Output x
    Number x_;
    // Output y
    Number y_;
    // Output z
    Number z_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
