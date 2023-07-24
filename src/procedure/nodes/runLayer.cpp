// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/runLayer.h"
#include "module/layer.h"
#include "procedure/nodes/node.h"
#include "keywords/layer.h"

RunLayerNode::RunLayerNode(const ModuleLayer* layer) : ProcedureNode(ProcedureNode::NodeType::RunLayer, {ProcedureNode::ControlContext}), layer_(layer)
{

    keywords_.setOrganisation("Options", "Target");
    keywords_.add<LayerKeyword>("Layer", "Target layer to run", layer_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RunLayerNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool RunLayerNode::prepare(const ProcedureContext &procedureContext)
{
    if (!layer_->isEnabled())
        return false;
}

// Execute node
bool RunLayerNode::execute(const ProcedureContext &procedureContext)
{
    if (!layer_->isEnabled())
    {
        Messenger::warn("Layer {} is disabled, so it won't be run!", layer_->name());
        return true;
    }

}
