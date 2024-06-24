// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "generator/nodeValue.h"

// Box Node
class BoxGeneratorNode : public GeneratorNode
{
    public:
    BoxGeneratorNode(Vec3<NodeValue> lengths = {1.0, 1.0, 1.0}, Vec3<NodeValue> angles = {90, 90, 90},
                     bool nonPeriodic = false);
    ~BoxGeneratorNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Node Data
     */
    private:
    // Box angles
    Vec3<NodeValue> angles_;
    // Box lengths
    Vec3<NodeValue> lengths_;
    // Whether the box is non-periodic
    bool nonPeriodic_{false};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
