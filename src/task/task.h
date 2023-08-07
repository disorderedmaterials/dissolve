// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "task/context.h"
#include "procedure/procedure.h"

class Task
{
    public:
    Task(TaskContext taskContext);
    ~Task() = default;

    private:
    // Procedure to be executed by the task
    Procedure* procedure_;
    // Context to execute the task within
    TaskContext context_;

    public:
    // Execute the task
    bool execute();

};