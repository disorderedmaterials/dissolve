// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "module/module.h"
#include "nodes/parameter.h"
#include "nodes/parameterLink.h"
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
    // Perform processing
    virtual Module::ExecutionResult process() { return Module::ExecutionResult::Failed; }
    // Confirm that node data is up to date
    bool isSatisfied();
    // Tell node to recalculate results
    void invalidate();

    /*
     * Inputs
     */
    private:
    // Input parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> inputs_;
    // Inbound Links
    std::map<std::string_view, ParameterLink> links_;
    // Whether node needs to run to account for updated data
    bool satisfied_{false};

    public:
    // Link an input
    bool link(std::string_view name, ParameterBase &source)
    {

        // Confirm that this node hasn't already been linked
        if (std::find_if(links_.begin(), links_.end(), [name](const auto &it) { return name == it.second.sink().name(); }) !=
            links_.end())
            return false;

        // Create link
        auto link = ParameterLink::link(source, *inputs_[name]);

        // Ensure link is value
        if (!link)
            return false;

        links_.emplace(std::make_pair(name, *link));
        return true;
    }
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
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &inputs();

    /*
     * Processing
     */
    protected:
    // Tell node that results are up to date
    void validate();
    // Prepare for processing
    bool preprocess();
};
