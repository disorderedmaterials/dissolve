// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/task.h"
#include "task/context.h"
#include "procedure/procedure.h"
#include "procedure/nodes/runLayer.h"
#include "procedure/nodes/context.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include <memory>

bool Task::execute(TaskContext &taskContext)
{
    ProcedureContext context(taskContext.procedurePool(), taskContext.dissolve());
    return procedure_.execute(context);
}

void Task::addRunLayerNode(ModuleLayer* layer)
{
    //procedure_.createRootNode<RunLayerNode>(fmt::format("Run {}", layer->name()), layer);
}