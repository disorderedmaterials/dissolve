// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/task.h"


// Set name of task
void Task::setName(std::string_view name)
{
    name_ = name;
}

// Return name of task
std::string_view Task::name() const
{
    return name_;
}

// Set procedure that the task will execute
void Task::setProcedure(Procedure procedure)
{
    procedure_ = procedure;
}


// Return procedure that the task will execute
Procedure Task::procedure() const
{
    return procedure_;
}

// Run task in the specified context
bool Task::execute(const TaskContext &context)
{
    return false;
}
