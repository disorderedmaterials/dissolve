// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "procedure/procedure.h"
#include "task/context.h"
#include <string>

class CoreData;

class Task : public Serialisable<const CoreData &>
{
    public:
    Task();
    ~Task() = default;

    /*
     * Task Definition
     */
    private:
    // Name of task
    std::string name_{"Untitled Task"};
    // Procedure that the task will execute
    Procedure procedure_;
    // Target configurations for the task
    std::vector<Configuration *> configurations_;

    public:
    // Set name of task
    void setName(std::string_view name);
    // Return name of task
    std::string_view name() const;
    // Set procedure that the task will execute
    void setProcedure(Procedure procedure);
    // Return procedure that the task will execute
    Procedure procedure() const;
    // Set target configurations for the task
    void setConfigurations(std::vector<Configuration *> configurations);
    void setConfigurations(Configuration *configuration);
    // Return target configurations for the task
    std::vector<Configuration *> configurations();
    // Return target configuration for the task
    Configuration *configuration() const;
    // Return whether there just exist a single target configuration for the task
    bool singularTargetConfiguration() const;

    public:
    // Run task in the specified context
    bool execute(const TaskContext &context);

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};