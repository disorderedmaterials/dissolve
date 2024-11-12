// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/add.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class Species;
class RegionGeneratorNodeBase;

// AddPair Node
class AddPairGeneratorNode : public GeneratorNode
{
    public:
    explicit AddPairGeneratorNode(const Species *spA = nullptr, const Species *spB = nullptr, const NodeValue &population = 0,
                                  const NodeValue &density = 0.1,
                                  Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddPairGeneratorNode() override = default;

    private:
    // Set up keywords for node
    void setUpKeywords();

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
    // Action to take on the Box geometry / volume on addition of the species
    AddGeneratorNode::BoxActionStyle boxAction_{AddGeneratorNode::BoxActionStyle::AddVolume};
    // Target density when adding molecules
    std::pair<NodeValue, Units::DensityUnits> density_{1.0, Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    NodeValue population_{1.0};
    // Positioning type for individual molecules
    AddGeneratorNode::PositioningType positioningType_{AddGeneratorNode::PositioningType::Random};
    // Region into which we will add molecules (if any)
    std::shared_ptr<const RegionGeneratorNodeBase> region_{nullptr};
    // Whether to rotate molecules on insertion
    bool rotate_{true};
    // Flags controlling box axis scaling
    bool scaleA_{true}, scaleB_{true}, scaleC_{true};
    // Species to be added
    const Species *speciesA_{nullptr}, *speciesB_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
