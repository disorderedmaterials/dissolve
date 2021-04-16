// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class Species;

// AddSpecies Node
class AddSpeciesProcedureNode : public ProcedureNode
{
    public:
    AddSpeciesProcedureNode(Species *sp = nullptr, NodeValue population = 0, NodeValue density = 0.1,
                            Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddSpeciesProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Node Data
     */
    public:
    // Box Action Style
    enum BoxActionStyle
    {
        None,        /* Box geometry / volume will remain unchanged */
        AddVolume,   /* Increase Box volume to accommodate new species, according to supplied density */
        ScaleVolume, /* Scale current Box volume to give, after addition of the current species, the supplied density */
        nBoxActionStyles
    };
    // Return enum option info for BoxActionStyle
    static EnumOptions<BoxActionStyle> boxActionStyles();
    // Positioning Type
    enum PositioningType
    {
        CentralPositioning, /* Position the Species at the centre of the Box */
        CurrentPositioning, /* Use current Species coordinates */
        RandomPositioning,  /* Set position randomly */
        nPositioningTypes
    };
    // Return enum option info for PositioningType
    static EnumOptions<PositioningType> positioningTypes();

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
