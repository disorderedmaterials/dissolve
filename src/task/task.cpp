// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "task/task.h"
#include "keywords/configurationVector.h"

Task::Task() : procedure_(ProcedureNode::NodeContext::AnyContext) {}

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

// Set target configurations for the task
void Task::setConfigurations(std::vector<std::unique_ptr<Configuration>> &configurations)
{
    configurations_ = configurations;
}

void Task::setConfigurations(std::unique_ptr<Configuration> configuration)
{
    configurations_.clear();
    configurations_.push_back(std::move(configuration));
}

// Return target configurations for the task
std::vector<std::unique_ptr<Configuration>> &Task::configurations() const
{
    return configurations_;
}

// Return target configuration for the task
std::unique_ptr<Configuration> Task::configuration() const
{
    if (!singularTargetConfiguration())
        throw(std::runtime_error("Task has multiple target configurations.\n"));
    return configurations_.front();
}

// Return whether there just exist a single target configuration for the task
bool Task::singularTargetConfiguration() const
{
    return configurations_.size() == 1;
}

// Run task in the specified context
bool Task::execute(const TaskContext &context)
{
    return procedure_.execute({context.dissolve(), context.processPool(), configuration()});
}

// Express as a serialisable value
SerialisedValue Task::serialise() const
{
    SerialisedValue result;
    result["name"] = name_;
    result["procedure"] = procedure_.serialise();
    result["targets"] = ConfigurationVectorKeyword(configurations_).serialise();
    return result;
}

// Read values from a serialisable value
void Task::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    name_ = toml::find<std::string>(node, "name");
    procedure_.deserialise(node.at("procedure"), coreData);
    configurations_ = ConfigurationVectorKeyword().deserialise(node.at("targets"), coreData).data();
}