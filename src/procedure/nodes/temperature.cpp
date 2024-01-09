// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/temperature.h"
#include "classes/configuration.h"
#include "keywords/nodeValue.h"
#include "procedure/nodes/node.h"

TemperatureProcedureNode::TemperatureProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::Temperature, {ProcedureNode::GenerationContext})
{
    keywords_.add<NodeValueKeyword>("Temperature", "Temperature (K)", temperature_, this);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool TemperatureProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Execute node
bool TemperatureProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setTemperature(temperature_.asDouble());
    return true;
}
