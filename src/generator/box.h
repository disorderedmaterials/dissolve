// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "generator/nodeValue.h"

// Box Node
class BoxGeneratorNode : public GeneratorNode
{
    public:
    BoxGeneratorNode(Vector3NodeValue lengths = {1.0, 1.0, 1.0}, Vector3NodeValue angles = {90, 90, 90},
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
    Vector3NodeValue angles_;
    // Box lengths
    Vector3NodeValue lengths_;
    // Whether the box is non-periodic
    bool nonPeriodic_{false};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
