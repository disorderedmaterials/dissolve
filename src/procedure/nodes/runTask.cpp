// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/runTask.h"
#include "task/task.h"

RunTaskNode::RunTaskNode(Task *task)
    : ProcedureNode(ProcedureNode::NodeType::RunTask, {ProcedureNode::ControlContext}), task_(task)
{
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RunTaskNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool RunTaskNode::prepare(const ProcedureContext &procedureContext) { return task_ != nullptr; }

// Execute node
bool RunTaskNode::execute(const ProcedureContext &procedureContext)
{
    if (!task_->execute({procedureContext.dissolve(), procedureContext.processPool(), procedureContext.configuration()}))
        return Messenger::error("Task '{}' experienced problems. Exiting now.\n", task_->name());

    return true;
}