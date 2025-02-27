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
    std::map<std::string, std::shared_ptr<ParameterBase>> inputs_;

    public:
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addInput(std::string_view name, std::string_view description, T &data)
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
    }
    // Add bounded input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedInput(std::string_view name, std::string_view description, T &data,
                                                   std::optional<T> lower = {}, std::optional<T> upper = {},
                                                   std::optional<T> step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        // Create new parameter using the supplied arguments
        // parameter->setBaseInfo(name, description);

        return inputs_.emplace(std::make_pair(name, new BoundedParameter<T>(this, name, description, data, lower, upper, step)))
            .first->second;
    }
    // Add bounded optional input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedOptionalInput(std::string_view name, std::string_view description, T &data,
                                                           T lower, std::string_view textWhenNull, T upper = {}, T step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_
            .emplace(std::make_pair(
                name, new BoundedOptionalParameter<T>(this, name, description, data, lower, textWhenNull, upper, step)))
            .first->second;
    }
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findInput(std::string_view name) const;
    // Return input parameters
    std::map<std::string, std::shared_ptr<ParameterBase>> &inputs();

    /*
     * Processing
     */
    private:
    // TODO
};
