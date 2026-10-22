// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"

class Vector3AssembleNode : public Node
{
    public:
    Vector3AssembleNode(Graph *parentGraph);
    ~Vector3AssembleNode() override = default;

    /*
     * Definition
     */
    private:
    // Input x
    double x_;
    // Input y
    double y_;
    // Input z
    double z_;
    // Output vector
    Vector3 outputVector_;

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
