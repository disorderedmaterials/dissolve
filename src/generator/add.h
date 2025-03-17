// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/addBase.h"
#include "generator/nodeValue.h"

// Forward Declarations
class CoordinateSetsGeneratorNode;
class Species;
class RegionGeneratorNodeBase;

// Add Node
class AddGeneratorNode : public AddGeneratorNodeBase
{
    public:
    explicit AddGeneratorNode(const Species *sp = nullptr, const NodeValue &population = 0, const NodeValue &density = 0.1,
                              Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    explicit AddGeneratorNode(std::shared_ptr<const CoordinateSetsGeneratorNode> sets, const NodeValue &population = 0,
                              const NodeValue &density = 0.1, Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddGeneratorNode() override = default;

    private:
    // Set up keywords for node
    void setUpKeywords();

    /*
     * Node Data
     */
    private:
    // Species to be added
    const Species *species_{nullptr};
    // Coordinate set source for Species (if any)
    std::shared_ptr<const CoordinateSetsGeneratorNode> coordinateSets_{nullptr};
    // Whether to rotate molecules on insertion
    bool rotate_{true};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
