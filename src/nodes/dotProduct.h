// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include "nodes/node.h"

// DotProductNode Node
class DotProductNode : public Node
{
    public:
    DotProductNode();
    ~DotProductNode() override = default;

    public:
    std::string_view name() override;
    std::string_view summary() override;

    /*
     * Definition
     */
    private:
    // Input vector 1
    Vec3<double> v1_;
    // Input vector 2
    Vec3<double> v2_;
    // Dot product value
    double dotProduct_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
