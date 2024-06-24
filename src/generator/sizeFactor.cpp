// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/sizeFactor.h"
#include "classes/configuration.h"
#include "generator/node.h"
#include "keywords/nodeValue.h"

SizeFactorGeneratorNode::SizeFactorGeneratorNode() : GeneratorNode(NodeType::SizeFactor)
{
    keywords_.add<NodeValueKeyword>("SizeFactor", "Size factor scaling to apply to configuration", sizeFactor_, this);
}

/*
 * Execute
 */

// Execute node
bool SizeFactorGeneratorNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setRequestedSizeFactor(sizeFactor_.asDouble());
    return true;
}
