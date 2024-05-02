// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/regionalGlobalPotential.h"
#include "classes/configuration.h"
#include "kernels/potentials/regional.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"

RegionalGlobalPotentialProcedureNode::RegionalGlobalPotentialProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::RegionalGlobalPotential, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<NodeValueKeyword>("Expression", "Expression describing region", expression_, this);
    keywords_.add<DoubleKeyword>("Minimum", "Minimum value for descriptive function defining region", minimumValue_);
    keywords_.add<DoubleKeyword>("Maximum", "Maximum value for descriptive function defining region", maximumValue_);
    keywords_.add<DoubleKeyword>("Offset", "Offset to apply to calculated value when assessing threshold and penalty",
                                 valueOffset_);
    keywords_.add<DoubleKeyword>("PenaltyPower", "Power to apply to the (offset) value if out of threshold range",
                                 penaltyPower_);

    keywords_.setOrganisation("Options", "Grid");
    keywords_.add<DoubleKeyword>("VoxelSize", "Voxel size (length) guiding the coarseness / detail of the region", voxelSize_,
                                 0.1);
}

/*
 * Execute
 */

// Execute node
bool RegionalGlobalPotentialProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    auto pot = std::make_unique<RegionalPotential>();
    pot->setUp(cfg->box(), voxelSize_,
               [&]()
               {
                   return std::make_shared<RegionalPotentialVoxelKernel>(expression_.asString(), getParameters(), minimumValue_,
                                                                         maximumValue_, valueOffset_, penaltyPower_);
               });

    cfg->addGlobalPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));

    return true;
}
