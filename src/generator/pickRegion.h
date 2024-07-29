// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/pickBase.h"

// Forward Declarations
class RegionGeneratorNodeBase;

// Pick Region Node
class PickRegionGeneratorNode : public PickGeneratorNodeBase
{
    public:
    explicit PickRegionGeneratorNode(std::shared_ptr<const RegionGeneratorNodeBase> region = nullptr);
    ~PickRegionGeneratorNode() override = default;

    /*
     * Data
     */
    private:
    // Target region for the pick
    std::shared_ptr<const RegionGeneratorNodeBase> region_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
