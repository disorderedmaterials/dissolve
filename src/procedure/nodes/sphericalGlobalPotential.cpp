// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/sphericalGlobalPotential.h"
#include "classes/configuration.h"
#include "keywords/bool.h"
#include "keywords/interactionPotential.h"
#include "keywords/vec3NodeValue.h"

SphericalGlobalPotentialProcedureNode::SphericalGlobalPotentialProcedureNode()
    : ProcedureNode(NodeType::SphericalGlobalPotential), potential_(Functions1D::Form::Harmonic)
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<InteractionPotentialKeyword<Functions1D>>("Potential", "Form of global potential to apply ", potential_);
    keywords_.add<Vec3NodeValueKeyword>("Origin", "Origin of global potential", origin_, this);
    keywords_.add<BoolKeyword>("Fractional", "Whether the origin is specified in fractional cell coordinates",
                               originIsFractional_);
}

/*
 * Execute
 */

// Execute node
bool SphericalGlobalPotentialProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();

    // Set potential
    auto pot = std::make_unique<SphericalPotential>();
    pot->setPotential(potential_);

    // Set origin
    Vec3<double> origin{origin_.x.asDouble(), origin_.y.asDouble(), origin_.z.asDouble()};
    if (originIsFractional_)
        cfg->box()->toReal(origin);
    pot->setOrigin(origin);

    cfg->addGlobalPotential(std::unique_ptr<ExternalPotential>(std::move(pot)));

    return true;
}
