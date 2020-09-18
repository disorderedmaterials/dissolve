/*
    *** Procedure Node - AddSpecies
    *** src/procedure/nodes/addspecies.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    ~AddSpeciesProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);
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
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
