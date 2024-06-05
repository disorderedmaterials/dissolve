// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/cylindricalGlobalPotential.h"
#include "classes/configuration.h"
#include "kernels/potentials/cylindrical.h"
#include "keywords/bool.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3NodeValue.h"

CylindricalGlobalPotentialProcedureNode::CylindricalGlobalPotentialProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::CylindricalGlobalPotential, {ProcedureNode::GenerationContext}),
      potential_(Functions1D::Form::LennardJones126)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Potential", "Form of directional global potential to apply ",
                                                            potential_);
    keywords_.add<Vec3NodeValueKeyword>("Origin", "Origin of global potential", origin_, this);
    keywords_.add<BoolKeyword>("Fractional", "Whether the origin is specified in fractional cell coordinates",
                               originIsFractional_);
    keywords_.add<Vec3NodeValueKeyword>("Vector", "Vector of global potential", vector_, this);
}

/*
 * Execute
 */

// Execute node
bool CylindricalGlobalPotentialProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    // Set potential
    auto pot = std::make_unique<CylindricalPotential>();
    pot->setPotential(potential_);

    // Set origin
    Vec3<double> origin{origin_.x.asDouble(), origin_.y.asDouble(), origin_.z.asDouble()};
    if (originIsFractional_)
        cfg->box()->toReal(origin);
    pot->setOrigin(origin);

    // Set directional vector
    pot->setVector({vector_.x.asDouble(), vector_.y.asDouble(), vector_.z.asDouble()});

    cfg->addGlobalPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));

    return true;
}
