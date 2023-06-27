// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotate.h"
#include "keywords/node.h"
#include "keywords/vec3nodevalue.h"
#include "procedure/nodes/select.h"

RotateProcedureNode::RotateProcedureNode(std::shared_ptr<SelectProcedureNode> site)
: ProcedureNode(ProcedureNode::NodeType::Rotate, {ProcedureNode::GenerationContext}), site_(site)
{
    keywords_.setOrganisation("Options", "Site");
    //keywords_.add<NodeKeyword<SelectProcedureNode>>("Site", "Site to be rotated", site_, this, ProcedureNode::NodeType::Select, true);
    keywords_.add<Vec3NodeValueKeyword>("Origin", "Origin to rotate to", origin_, this);
}

bool RotateProcedureNode::execute(const ProcedureContext &procedureContext)
{
    Messenger::print("{}", site_->name());
    return true;
}

