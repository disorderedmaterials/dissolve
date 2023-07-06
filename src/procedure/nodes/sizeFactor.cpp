// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/sizeFactor.h"
#include "classes/configuration.h"
#include "keywords/nodeValue.h"
#include "procedure/nodes/node.h"
SizeFactorProcedureNode::SizeFactorProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::SizeFactor, {ProcedureNode::GenerationContext})
{
    keywords_.add<NodeValueKeyword>("SizeFactor", "Size factor for scaling", sizeFactor_, this);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool SizeFactorProcedureNode::mustBeNamed() const { return true; }

/*
 * Execute
 */

// Execute node
bool SizeFactorProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setRequestedSizeFactor(sizeFactor_.asDouble());
    cfg->applySizeFactor(procedureContext.processPool(), procedureContext.potentialMap());
    return true;
}
