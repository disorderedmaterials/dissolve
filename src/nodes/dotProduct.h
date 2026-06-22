// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"
#include "nodes/number.h"

class DotProductNode : public Node
{
    public:
    DotProductNode(Graph *parentGraph);
    ~DotProductNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Input vector 1
    Vector3 u_;
    // Input vector 2
    Vector3 v_;
    // Dot product value
    Number dotProduct_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
