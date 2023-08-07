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
#include "module/layer.h"
#include "fmt/format.h"

bool Task::execute(TaskContext &taskContext)
{
    ProcedureContext context(taskContext.processPool(), taskContext.dissolve());
    return procedure_.execute(context);
}

void Task::addRunLayerNode(ModuleLayer *layer)
{
    auto run = procedure_.createRootNode<RunLayerNode>(fmt::format("Run {}", layer->name()), layer);
    Messenger::print("Added node {}, targets {}", run->name(), run->keywords().keyword("Layer")->name());
}