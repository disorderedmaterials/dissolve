// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/rotate.h"
#include "keywords/vec3nodevalue.h"

RotateNode::RotateNode()
    : ProcedureNode(ProcedureNode::NodeType::Rotate, {ProcedureNode::GenerationContext})
{
    keywords_.setOrganisation("Options", "Definition");
    keywords_.add<Vec3NodeValueKeyword>("Origin", "Origin to rotate to", origin_, this);
}

bool RotateNode::execute(const ProcedureContext &procedureContext)
{
    return true;
}

