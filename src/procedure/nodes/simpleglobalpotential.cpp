// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/simpleglobalpotential.h"
#include "classes/configuration.h"
#include "keywords/interactionpotential.h"
#include "keywords/vec3nodevalue.h"

SimpleGlobalPotentialProcedureNode::SimpleGlobalPotentialProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::SimpleGlobalPotential), potential_(SimplePotentialFunctions::Form::Harmonic)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<InteractionPotentialKeyword<SimplePotentialFunctions>>("Potential", "Form of global potential to apply ",
                                                                         potential_);
    keywords_.add<Vec3NodeValueKeyword>("Origin", "Origin of global potential", origin_, this);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool SimpleGlobalPotentialProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

/*
 * Execute
 */

// Execute node
bool SimpleGlobalPotentialProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    auto pot = std::make_unique<SimplePotential>();
    pot->setPotential(potential_);
    pot->setOrigin({origin_.x.asDouble(), origin_.y.asDouble(), origin_.z.asDouble()});

    cfg->addGlobalPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));

    return true;
}
