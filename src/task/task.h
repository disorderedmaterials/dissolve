// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "task/context.h"
#include "base/serialiser.h"
#include "classes/coreData.h"
#include "procedure/procedure.h"
#include <string>


class Task : public Serialisable<const CoreData &>
{
    public:
    Task() = default;
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

};