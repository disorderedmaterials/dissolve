// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <memory>
#include <string>
#include <vector>
#include "module/module.h"
#include "task/context.h"

class CoreData;
class Procedure;

class Task : public Serialisable<const CoreData &>
{
    public:
    Task() = default;
    ~Task() = default;

    private:
    std::string name{"Untitled Task"};
    TaskContext context_;

    /*
     * Modules
     */
    private:
    // Vector of Modules in the layer
    std::vector<std::unique_ptr<Module>> modules_;

    public:
    // Return vector of Modules
    std::vector<std::unique_ptr<Module>> &modules();
    Module *addModule(ModuleTypes::ModuleType moduleType);

    public:
    bool prepare();
    bool execute();

    /*
     * General Actions
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};