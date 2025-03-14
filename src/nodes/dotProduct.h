// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "templates/vector3.h"

// DotProductNode Node
class DotProductNode : public Node
{
    public:
    DotProductNode();
    ~DotProductNode() override = default;

    public:
    std::string_view name() const override;
    std::string_view summary() const override;

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
