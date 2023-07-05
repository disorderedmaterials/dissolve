// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/temperature.h"
#include "procedure/nodes/node.h"
#include "keywords/nodeValue.h"
#include "classes/configuration.h"
TemperatureProcedureNode::TemperatureProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Temperature, {ProcedureNode::GenerationContext})
{
    keywords_.add<NodeValueKeyword>("Temperature", "Temperature in K", temperature_, this);
}

bool TemperatureProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto *cfg = procedureContext.configuration();
    cfg->setTemperature(temperature_.asDouble());
    return true;
}
