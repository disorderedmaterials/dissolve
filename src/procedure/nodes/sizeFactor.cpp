// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/sizeFactor.h"
#include "procedure/nodes/node.h"
#include "keywords/nodeValue.h"
#include "classes/configuration.h"
SizeFactorProcedureNode::SizeFactorProcedureNode() : ProcedureNode(ProcedureNode::NodeType::SizeFactor, {ProcedureNode::GenerationContext})
{
    keywords_.add<NodeValueKeyword>("SizeFactor", "Size factor for scaling", sizeFactor_, this);
}

bool SizeFactorProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setRequestedSizeFactor(sizeFactor_.asDouble());
    return true;
}

