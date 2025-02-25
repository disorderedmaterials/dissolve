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
    Node() {}
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
    std::vector<std::shared_ptr<ParameterBase>> inputs_;

    public:
    // Add input parameter
    template <class T> void addInput(std::string_view name, std::string_view description, T &data, T defValue)
    {
        // Check for keyword of this name already
        //        if (find(name))
        //            Messenger::exception("Keyword named '{}' already exists, and can't be added again.", name);

        // Create new parameter using the supplied arguments
        // parameter->setBaseInfo(name, description);

        inputs_.emplace_back(new Parameter<T>(name, description, data, defValue));
    }
    // Add bounded input parameter
    template <class T>
    void addBoundedInput(std::string_view name, std::string_view description, T &data, T defValue, std::optional<T> lower = {},
                         std::optional<T> upper = {}, std::optional<T> step = {})
    {
        // Check for keyword of this name already
        //        if (find(name))
        //            Messenger::exception("Keyword named '{}' already exists, and can't be added again.", name);

        // Create new parameter using the supplied arguments
        // parameter->setBaseInfo(name, description);

        inputs_.emplace_back(new BoundedParameter<T>(name, description, data, defValue, lower, upper, step));
    }
    // Return input parameters
    std::vector<std::shared_ptr<ParameterBase>> &inputs() { return inputs_; };

    /*
     * Processing
     */
    private:
    // TODO
};
