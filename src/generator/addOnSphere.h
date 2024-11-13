// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/add.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class CoordinateSetsGeneratorNode;
class Species;
class RegionGeneratorNodeBase;

// Add Node
class AddOnSphereGeneratorNode : public AddGeneratorNodeBase
{
    public:
    explicit AddOnSphereGeneratorNode(const SpeciesSite *site = nullptr, const NodeValue &population = 0,
                                      const NodeValue &density = 0.1,
                                      Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddOnSphereGeneratorNode() override = default;

    private:
    // Set up keywords for node
    void setUpKeywords();

    /*
     * Node Data
     */
    public:
    // Point Distribution Style
    enum class PointDistributionStyle
    {
        Random,   /* Points are generated randomly */
        Fibonacci /* Points are generated on a Fibonacci lattice */
    };
    // Return enum option info for PointDistributionStyle
    static EnumOptions<PointDistributionStyle> pointDistributionStyles();

    private:
    // Site representing anchor point (and implicitly the target species)
    const SpeciesSite *speciesSite_{nullptr};
    // Radius of the underlying sphere
    NodeValue radius_{5.0};
    // Distribution style for points on the sphere
    PointDistributionStyle pointDistributionStyle_{PointDistributionStyle::Fibonacci};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
