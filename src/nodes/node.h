// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/parameters/parameter.h"
#include <string>
#include <vector>

// Node Base
class Node
{
    public:
    Node();
    virtual ~Node() = default;

    /*
     * Definition (Virtuals)
     */
    public:
    // Return short name of the node
    virtual std::string_view name() = 0;
    // Return short summary of the node's purpose
    virtual std::string_view summary() = 0;

    /*
     * Inputs & Outputs
     */
    private:
    // Input parameters
    std::vector<Parameter*> inputs_;

    public:
    // Add input parameter
    template <class P, typename... Args> void addInput(std::string_view name, std::string_view description, Args &&...args)
    {
        // Check for keyword of this name already
//        if (find(name))
//            Messenger::exception("Keyword named '{}' already exists, and can't be added again.", name);

        // Create new parameter using the supplied arguments
        P *parameter = new P(std::forward<Args>(args)...);
        parameter->setBaseInfo(name, description);

        inputs_.push_back(parameter);
    }
    // Return input parameters
    std::vector<Parameter*> &inputs();

    /*
     * Processing
     */
    private:
    // TODO
};
