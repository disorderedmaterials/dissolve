// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <memory>
#include <string>
#include <vector>
#include "module/module.h"
#include "procedure/procedure.h"
#include "task/context.h"

class CoreData;

class Task : public Serialisable<const CoreData &>
{
    public:
    Task() = default;
    ~Task() = default;

    private:
    std::string name{"Untitled Task"};
    Procedure procedure_;

    public:
    bool execute(TaskContext& taskContext);
};