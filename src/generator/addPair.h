// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/addBase.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class Species;
class RegionGeneratorNodeBase;

// AddPair Node
class AddPairGeneratorNode : public AddGeneratorNodeBase
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
     * Node Data
     */
    private:
    // Species to be added
    const Species *speciesA_{nullptr}, *speciesB_{nullptr};
    // Whether to rotate molecules on insertion
    bool rotate_{true};
    // Flags controlling box axis scaling
    bool scaleA_{true}, scaleB_{true}, scaleC_{true};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
