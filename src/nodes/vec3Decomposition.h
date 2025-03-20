// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "templates/vector3.h"

// Vec3DecompositionNode Node
class Vec3DecompositionNode : public Node
{
    public:
    Vec3DecompositionNode();
    ~Vec3DecompositionNode() override = default;

    public:
    std::string_view name() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Input vector
    Vec3<double> inputVector_;
    // Output x
    double x_;
    // Output y
    double y_;
    // Output z
    double z_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
