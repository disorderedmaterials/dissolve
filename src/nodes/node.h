// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "base/serialiser.h"
#include "module/module.h"
#include "nodes/constants.h"
#include "nodes/parameter.h"
#include <map>
#include <string>
#include <vector>

// Forward Declarations
class Graph;
class Edge;

// Node Base
class Node : public Serialisable<>
{
    public:
    Node() {}
    explicit Node(Graph *parentGraph) : parentGraph_(parentGraph) {}
    virtual ~Node() = default;

    using EdgeMap = std::map<std::string_view, Edge *>;

    /*
     * Definition
     */
    private:
    // Name of the node (unique within it's parent Graph)
    std::string name_;
    // Node parent graph
    Graph *parentGraph_;

    public:
    // Set node name
    void setName(std::string_view newName);
    // Return node name
    virtual std::string_view name() const;
    // Return node type
    virtual std::string_view type() const = 0;
    // Return short summary of the node's purpose
    virtual std::string_view summary() const = 0;

    /*
     * Node message
     */

    // Enumeration for message status
    enum class MessageStatus
    {
        Info,
        Warn,
        Error
    };

    using MessageStore = std::vector<std::pair<MessageStatus, std::string>>;
    // Print latest message
    static bool echo_;

    private:
    // Message store vector
    MessageStore messages_;
    // Node message format
    template <typename... Args> void message(std::format_string<Args...> format, Args... args)
    {
        messages_.emplace_back(std::make_pair(MessageStatus::Info, std::format(format, std::forward<Args>(args)...)));

        if (echo_)
            echo();
    }
    // Node warn format
    template <typename... Args> void warn(std::format_string<Args...> format, Args... args)
    {
        messages_.emplace_back(std::make_pair(MessageStatus::Warn, std::format(format, std::forward<Args>(args)...)));

        if (echo_)
            echo();
    }
    // Node error format
    template <typename... Args> void error(std::format_string<Args...> format, Args... args)
    {
        messages_.emplace_back(std::make_pair(MessageStatus::Error, std::format(format, std::forward<Args>(args)...)));

        if (echo_)
            echo();
    }
    // Print latest message
    void echo();

    public:
    // Message store vector
    const MessageStore &messages() const;
    // Returns true if message with given status exists
    bool hasMessages(MessageStatus status) const;

    /*
     * Processing & Validity
     */
    private:
    // Version index for the node, bumped whenever result outputs change
    int versionIndex_{NodeConstants::InvalidVersion};
    // Whether the node's data is up-to-date
    bool upToDate_{false};

    protected:
    // Perform processing
    virtual NodeConstants::ProcessResult process();

    public:
    // Return version index for the node, bumped whenever result outputs change
    int versionIndex() const;
    // Invalidate the current node, resetting versionIndex_
    void invalidate();
    // Flag that the node data needs to be updated
    void setUpdateRequired();
    // Return whether the node's data is up-to-date
    bool isUpToDate() const;
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
    // Link edge, returning whether we accept it
    bool linkEdge(Edge *edge);
    // Unlink edge
    void unlinkEdge(Edge *edge);
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
    // Add pointer input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addPointerInput(std::string_view name, std::string_view description, T *data)
    {
        if (findInput(name))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", name);

        auto param =
            inputs_.emplace(std::make_pair(name, new PointerParameter<T>(this, name, description, data))).first->second;
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
    // Add pointer output parameter
    template <typename T>
    std::shared_ptr<ParameterBase> addPointerOutput(std::string_view name, std::string_view description,
                                                    std::remove_pointer<T>::type &object)
    {
        if (findOutput(name))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", name);

        auto param =
            outputs_.emplace(std::make_pair(name, new PointerParameter<T>(this, name, description, object))).first->second;
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
    // Returns the node parent graph
    Graph *parentGraph() const;
    // Return the Dissolve reference
    virtual Dissolve &dissolve() const;
    // Return the world pool
    virtual const ProcessPool &processPool() const;

    /*
     * Data
     */
    public:
    // Clear any local data
    virtual void clearData();

    /*
     * I/O
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
