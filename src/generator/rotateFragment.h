// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "generator/node.h"

class RotateFragmentGeneratorNode : public GeneratorNode
{
    public:
    RotateFragmentGeneratorNode(std::shared_ptr<SelectGeneratorNode> site = nullptr);
    ~RotateFragmentGeneratorNode() override = default;

    /*
     * Identity
     */
    private:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Control
     */
    private:
    // Site to be rotated
    std::shared_ptr<const SelectGeneratorNode> site_;
    // Rotation
    NodeValue rotation_{90.0};
    // Axis
    Site::SiteAxis axis_{Site::SiteAxis::XAxis};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
