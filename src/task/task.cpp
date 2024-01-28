// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "task/task.h"
#include "classes/configuration.h"

Task::Task() : procedure_(ProcedureNode::NodeContext::AnyContext) {}

// Set name of task
void Task::setName(std::string_view name) { name_ = name; }

// Return name of task
std::string_view Task::name() const { return name_; }

// Set procedure that the task will execute
void Task::setProcedure(Procedure procedure) { procedure_ = procedure; }

// Return procedure that the task will execute
Procedure Task::procedure() const { return procedure_; }

// Set target configurations for the task
void Task::setConfigurations(std::vector<Configuration *> configurations) { configurations_ = configurations; }

void Task::setConfigurations(Configuration *configuration)
{
    configurations_.clear();
    configurations_.push_back(configuration);
}

// Return target configurations for the task
std::vector<Configuration *> Task::configurations() { return configurations_; }

// Return target configuration for the task
Configuration *Task::configuration() const
{
    if (!singularTargetConfiguration())
        throw(std::runtime_error("Task has multiple target configurations.\n"));
    return configurations_.front();
}

// Return whether there just exist a single target configuration for the task
bool Task::singularTargetConfiguration() const { return configurations_.size() == 1; }

// Run task in the specified context
bool Task::execute(const TaskContext &context)
{
    return procedure_.execute({context.dissolve(), context.processPool(), configuration()});
}

// Express as a serialisable value
SerialisedValue Task::serialise() const
{
    SerialisedValue result;
    Serialisable::fromVectorToTable(configurations_, "targets", result);
    result["procedure"] = procedure_.serialise();
    return result;
}

// Read values from a serialisable value
void Task::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    // Serialisable::toMap(node, "targets",
    //                     [&coreData, this](const std::string &name, const SerialisedValue &data)
    //                     {
    //                         configurations_.push_back(coreData.findConfiguration(name));
    //                     }
    // );
    return;
}