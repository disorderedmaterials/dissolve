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
    private:
    // The default scaling settings
    static constexpr bool defaultScale_{true};
    // iFlags controlling box axis scaling
    bool scaleA_{defaultScale_}, scaleB_{defaultScale_}, scaleC_{defaultScale_};
    // Site representing anchor point (and implicitly the target species)
    const SpeciesSite *speciesSite_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const GeneratorContext &generatorContext) override;
    // Execute node
    bool execute(const GeneratorContext &generatorContext) override;
};
