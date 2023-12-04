// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/sizeFactor.h"
#include "classes/configuration.h"
#include "keywords/nodeValue.h"
#include "procedure/nodes/node.h"

SizeFactorProcedureNode::SizeFactorProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::SizeFactor, {ProcedureNode::GenerationContext})
{
    keywords_.add<NodeValueKeyword>("SizeFactor", "Size factor scaling to apply to configuration", sizeFactor_, this);
}

/*
 * Execute
 */

// Execute node
bool SizeFactorProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setRequestedSizeFactor(sizeFactor_.asDouble());
    return true;
}
