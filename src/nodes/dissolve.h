// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"

// AtomShake Node
class DissolveNode : public Node
{
    public:
    DissolveNode(){};
    ~DissolveNode() override = default;

    public:
    std::string_view name() const override { return "Dissolve"; }
    std::string_view summary() const override { return "Parent node of all simulations"; }

    /*
     * Definition
     */
    private:
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
