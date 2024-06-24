// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/temperature.h"
#include "classes/configuration.h"
#include "generator/node.h"
#include "keywords/nodeValue.h"

TemperatureGeneratorNode::TemperatureGeneratorNode() : GeneratorNode(NodeType::Temperature)
{
    keywords_.add<NodeValueKeyword>("Temperature", "Temperature (K)", temperature_, this);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool TemperatureGeneratorNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node
bool TemperatureGeneratorNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setTemperature(temperature_.asDouble());
    return true;
}
