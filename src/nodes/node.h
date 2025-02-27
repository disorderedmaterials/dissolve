// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "nodes/parameter.h"
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
     * Inputs
     */
    private:
    // Input parameters
    std::vector<std::shared_ptr<ParameterBase>> inputs_;

    public:
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addInput(std::string_view name, std::string_view description, T &data)
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace_back(new Parameter<T>(name, description, data));
    }
    // Add bounded input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedInput(std::string_view name, std::string_view description, T &data,
                                                   std::optional<T> lower = {}, std::optional<T> upper = {},
                                                   std::optional<T> step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace_back(new BoundedParameter<T>(name, description, data, lower, upper, step));
    }
    // Add bounded optional input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedOptionalInput(std::string_view name, std::string_view description, T &data,
                                                           T lower, std::string_view textWhenNull, T upper = {}, T step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace_back(new BoundedOptionalParameter<T>(name, description, data, lower, textWhenNull, upper, step));
    }
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findInput(std::string_view name) const;
    // Return input parameters
    std::vector<std::shared_ptr<ParameterBase>> &inputs();

    /*
     * Processing
     */
    private:
    // TODO
};
