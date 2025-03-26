// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "base/serialiser.h"
#include "module/module.h"
#include "nodes/constants.h"
#include "nodes/edge.h"
#include "nodes/parameter.h"
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

    using EdgeMap = std::map<std::string_view, Edge *>;

    private:
    // Node parent graph
    Graph *parentGraph_;

    /*
     * Definition
     */
    public:
    // Return short name of the node
    virtual std::string_view name() const = 0;
    // Return short summary of the node's purpose
    virtual std::string_view summary() const = 0;

    /*
     * Processing & Validity
     */
    private:
    // Version index for the node, bumped whenever result outputs change
    int versionIndex_{NodeConstants::InvalidVersion};

    protected:
    // Perform processing
    virtual NodeConstants::ProcessResult process();

    public:
    // Return version index for the node, bumped whenever result outputs change
    int versionIndex() const;
    // Invalidate the current node, resetting versionIndex_
    void invalidate();
    // Check that all required inputs are present, and that all inputs are valid
    bool inputsAreValid() const;
    // Run the node, retrieving dependent inputs as necessary
    NodeConstants::ProcessResult run();

    /*
     * Inputs, Outputs, and Options
     */
    private:
    // Input parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> inputs_;
    // Output parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> outputs_;
    // Keyword options
    std::map<std::string_view, std::shared_ptr<ParameterBase>> options_;
    // Inbound edges
    EdgeMap inputEdges_;

    public:
    // Add edge, returning whether we accept it
    bool addEdge(Edge *edge);
    // Remove edge
    void removeEdge(Edge *edge);
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addOption(std::string_view name, std::string_view description, T &data)
    {
        if (findInput(name))
            Messenger::exception("Option '{}' already exists, and can't be added again.", name);

        auto param = options_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add input parameter
    template <class T> std::shared_ptr<ParameterBase> addInput(std::string_view name, std::string_view description, T &data)
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        auto param = inputs_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add bounded input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedInput(std::string_view name, std::string_view description, T &data,
                                                   std::optional<T> lower = {}, std::optional<T> upper = {},
                                                   std::optional<T> step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        auto param =
            inputs_.emplace(std::make_pair(name, new BoundedParameter<T>(this, name, description, data, lower, upper, step)))
                .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add bounded optional input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedOptionalInput(std::string_view name, std::string_view description, T &data,
                                                           T lower, std::string_view textWhenNull, T upper = {}, T step = {})
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        auto param = inputs_
                         .emplace(std::make_pair(name, new BoundedOptionalParameter<T>(this, name, description, data, lower,
                                                                                       textWhenNull, upper, step)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add output parameter
    template <class T> std::shared_ptr<ParameterBase> addOutput(std::string_view name, std::string_view description, T &data)
    {
        if (findOutput(name))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", name);

        auto param = outputs_.emplace(std::make_pair(name, new Parameter<T>(this, name, description, data))).first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findInput(std::string_view name) const;
    // Return input parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &inputs();
    // Return named output parameter if it exists
    std::shared_ptr<ParameterBase> findOutput(std::string_view name) const;
    // Return output parameters
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &outputs();
    // Return named option if it exists
    std::shared_ptr<ParameterBase> findOption(std::string_view name) const;
    // Return options
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &options();
    // Get the links owned by this node
    EdgeMap &links();
    // Set the node parent graph
    void setParentGraph(Graph *parentGraph);
    // Returns the node parent graph
    Graph *parentGraph() const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
