// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/task.h"
#include "task/context.h"
#include "procedure/procedure.h"
#include "procedure/nodes/context.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

bool Task::execute(TaskContext &taskContext)
{
    ProcedureContext context();
}