// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "base/serialiser.h"
#include "module/module.h"
#include "nodes/parameter.h"
#include "nodes/parameterLink.h"
#include <map>
#include <string>
#include <vector>

class Graph;

// Node Base
class Node : public Serialisable<>
{
    public:
    Node() {}
    explicit Node(Graph *parentGraph) : parentGraph_(parentGraph) {}
    virtual ~Node() = default;

    using LinkMap = std::map<std::string_view, ParameterLink>;

    enum class Readiness
    {
        Ready,
        MissingComponent,
    };

    private:
    // Node parent graph
    Graph *parentGraph_;

    /*
     * Definition (Virtuals)
     */
    public:
    // Return short name of the node
    virtual std::string_view name() const = 0;
    // Return short summary of the node's purpose
    virtual std::string_view summary() const = 0;
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
    // Keyword options
    std::map<std::string_view, std::shared_ptr<ParameterBase>> options_;
    // Inbound Links
    LinkMap inputLinks_;
    // Whether node needs to run to account for updated data
    bool satisfied_{false};

    public:
    // Link an input
    bool link(std::string_view name, ParameterBase &source)
    {

        // Confirm that this node hasn't already been linked
        if (std::find_if(inputLinks_.begin(), inputLinks_.end(),
                         [name](const auto &it) { return name == it.second.sink().name(); }) != inputLinks_.end())
            return false;

        // Confirm that the source is actually a source
        if (!source.flags().isSet(ParameterBase::ParameterFlags::Output))
            return Messenger::error("{} does not output data", source.name());

        // Confirm that the destination is actually a sink
        if (inputs_[name]->flags().isSet(ParameterBase::ParameterFlags::Output))
            return Messenger::error("{} does not accept data", source.name());

        // Create link
        auto link = ParameterLink::link(source, *inputs_[name]);

        // Ensure link is value
        if (!link)
            return false;

        inputLinks_.emplace(std::make_pair(name, *link));
        return true;
    }
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addOption(std::string_view name, std::string_view description, T &data)
    {
        if (findParameter(name))
            Messenger::exception("Option '{}' already exists, and can't be added again.", name);

        return options_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
    }
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addInput(std::string_view name, std::string_view description, T &data)
    {
        if (findParameter(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
    }
    // Add output parameter
    template <class T> std::shared_ptr<ParameterBase> addOutput(std::string_view name, std::string_view description, T &data)
    {
        if (findParameter(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        auto param = inputs_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Add bounded input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedInput(std::string_view name, std::string_view description, T &data,
                                                   std::optional<T> lower = {}, std::optional<T> upper = {},
                                                   std::optional<T> step = {})
    {
        if (findParameter(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_.emplace(std::make_pair(name, new BoundedParameter<T>(this, name, description, data, lower, upper, step)))
            .first->second;
    }
    // Add bounded optional input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedOptionalInput(std::string_view name, std::string_view description, T &data,
                                                           T lower, std::string_view textWhenNull, T upper = {}, T step = {})
    {
        if (findParameter(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        return inputs_
            .emplace(std::make_pair(
                name, new BoundedOptionalParameter<T>(this, name, description, data, lower, textWhenNull, upper, step)))
            .first->second;
    }
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findParameter(std::string_view name) const;
    // Return input parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &parameters();
    // Return named option if it exists
    std::shared_ptr<ParameterBase> findOption(std::string_view name) const;
    // Return options
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &options();
    // Get the links owned by this node
    LinkMap &links();
    // Set the node parent graph
    void setParentGraph(Graph *parentGraph);
    // Returns the node parent graph
    Graph *parentGraph() const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;

    /*
     * Processing
     */
    protected:
    // Tell node that results are up to date
    void validate();
    // Prepare for processing
    Readiness preprocess();
};
