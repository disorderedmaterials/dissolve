// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "templates/vector3.h"

// Vec3AssemblyNode Node
class Vec3AssemblyNode : public Node
{
    public:
    Vec3AssemblyNode();
    ~Vec3AssemblyNode() override = default;

    public:
    std::string_view name() const override;
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
    Vec3<double> outputVector_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
