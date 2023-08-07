// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/task.h"
#include "task/context.h"
#include "procedure/procedure.h"

bool Task::execute()
{
    ProcedureContext procedureContext(context_.processPool(), context_.dissolve());
    return procedure_.execute(procedureContext);
}