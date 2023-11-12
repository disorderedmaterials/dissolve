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
    return procedure_.execute({context.dissolve(), context.processPool(), context.configuration()});
}

// Express as a serialisable value
SerialisedValue Task::serialise() const
{
    SerialisedValue result;
    result["name"] = name_;
    result["procedure"] = procedure_.serialise();
    return result;
}

// Read values from a serialisable value
void Task::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    name_ = toml::find<std::string>(node, "name");
    procedure_.deserialise(node.at("procedure"), coreData);
}