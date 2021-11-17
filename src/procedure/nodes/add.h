// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class Species;
class RegionProcedureNodeBase;

// Add Node
class AddProcedureNode : public ProcedureNode
{
    public:
    AddProcedureNode(const Species *sp = nullptr, const NodeValue &population = 0, const NodeValue &density = 0.1,
                     Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
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
    AddProcedureNode::BoxActionStyle boxAction_{AddProcedureNode::BoxActionStyle::AddVolume};
    // Target density when adding molecules
    std::pair<NodeValue, Units::DensityUnits> density_{1.0, Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    NodeValue population_{1.0};
    // Positioning type for individual molecules
    AddProcedureNode::PositioningType positioningType_{AddProcedureNode::PositioningType::Random};
    // Region into which we will add molecules (if any)
    std::shared_ptr<const RegionProcedureNodeBase> region_{nullptr};
    // Whether to rotate molecules on insertion
    bool rotate_{true};
    // Flags controlling box axis scaling
    bool scaleA_{true}, scaleB_{true}, scaleC_{true};
    // Species to be added
    const Species *species_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
