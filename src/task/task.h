// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "task/context.h"
#include "base/serialiser.h"
#include "procedure/procedure.h"
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
    Procedure  procedure_;

    public:
    // Set name of task
    void setName(std::string_view name);
    // Return name of task
    std::string_view name() const;
    // Set procedure that the task will execute
    void setProcedure(Procedure procedure);
    // Return procedure that the task will execute
    Procedure procedure() const;

    public:
    // Run task in the specified context
    bool execute(const TaskContext &context);

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;

};