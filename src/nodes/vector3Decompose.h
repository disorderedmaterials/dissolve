// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"
#include "nodes/number.h"

class Vector3DecomposeNode : public Node
{
    public:
    Vector3DecomposeNode(Graph *parentGraph);
    ~Vector3DecomposeNode() override = default;

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

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
