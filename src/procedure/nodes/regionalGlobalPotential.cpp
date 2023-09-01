// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/regionalGlobalPotential.h"
#include "classes/configuration.h"
#include "kernels/potentials/regional.h"
#include "keywords/vec3NodeValue.h"

RegionalGlobalPotentialProcedureNode::RegionalGlobalPotentialProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::RegionalGlobalPotential, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Definition");
//    keywords_.add<InteractionPotentialKeyword<DirectionalPotentialFunctions>>(
//        "Potential", "Form of directional global potential to apply ", potential_);
}

/*
 * Execute
 */

// Execute node
bool RegionalGlobalPotentialProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    auto pot = std::make_unique<RegionalPotential>();
//    pot->setPotential(potential_);
//    pot->setOrigin({origin_.x.asDouble(), origin_.y.asDouble(), origin_.z.asDouble()});
//    pot->setVector({vector_.x.asDouble(), vector_.y.asDouble(), vector_.z.asDouble()});
    pot->setUp(cfg->box());

    cfg->addGlobalPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));

    return true;
}
