// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
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
    public:
    // Box Action Style
    enum class BoxActionStyle
    {
        None,        /* Box geometry / volume will remain unchanged */
        AddVolume,   /* Increase Box volume to accommodate new species, according to supplied density */
        ScaleVolume, /* Scale current Box volume to give, after addition of the current species, the supplied density */
        Set          /* Set the Box geometry to that specified in the Species */
    };
    // Return enum option info for BoxActionStyle
    static EnumOptions<BoxActionStyle> boxActionStyles();
    // Positioning Type
    enum class PositioningType
    {
        Central, /* Position the Species at the centre of the Box */
        Current, /* Use current Species coordinates */
        Random,  /* Set position randomly */
        Region   /* Set position randomly within a specified region */
    };
    // Return enum option info for PositioningType
    static EnumOptions<PositioningType> positioningTypes();

    private:
    // Action to take on the Box geometry / volume on addition of the species
    AddPairGeneratorNode::BoxActionStyle boxAction_{AddPairGeneratorNode::BoxActionStyle::AddVolume};
    // Target density when adding molecules
    std::pair<NodeValue, Units::DensityUnits> density_{1.0, Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    NodeValue population_{1.0};
    // Positioning type for individual molecules
    AddPairGeneratorNode::PositioningType positioningType_{AddPairGeneratorNode::PositioningType::Random};
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
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
