// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"

// Vec3Assembly Node
class Vec3AssemblyNode : public Node
{
    public:
    Vec3AssemblyNode(Graph *parentGraph);
    ~Vec3AssemblyNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

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

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
