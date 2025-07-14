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
    protected:
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

    protected:
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
    template <typename... Args> NodeConstants::ProcessResult error(std::format_string<Args...> format, Args... args)
    {
        messages_.emplace_back(std::make_pair(MessageStatus::Error, std::format(format, std::forward<Args>(args)...)));

        if (echo_)
            echo();

        return NodeConstants::ProcessResult::Failed;
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
    virtual void setUpdateRequired();
    // Return whether the node's data is up-to-date
    bool isUpToDate() const;
    // Check that all required inputs are present, and that all inputs are valid
    bool inputsAreValid() const;
    // Run the node, retrieving dependent inputs as necessary
    NodeConstants::ProcessResult run();

    /*
     * Inputs, Outputs, and Options
     */
    public:
    using NodeParameterMap = std::map<std::string, std::shared_ptr<ParameterBase>>;

    protected:
    // Input parameters
    NodeParameterMap inputs_;
    // Output parameters
    NodeParameterMap outputs_;
    // Keyword options
    NodeParameterMap options_;
    // Inbound edges
    EdgeMap inputEdges_;
    // Outgoing edges
    EdgeMap outputEdges_;

    public:
    // Link edge, returning whether we accept it
    bool linkEdge(Edge *edge);
    // Unlink edge
    void unlinkEdge(Edge *edge);
    // Add input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addOption(std::string_view optionName, std::string_view description, T &data)
    {
        if (findInput(optionName))
            Messenger::exception("Option '{}' already exists, and can't be added again.", optionName);

        auto param =
            options_.emplace(std::make_pair(optionName, std::make_shared<Parameter<T>>(this, optionName, description, data)))
                .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addInput(std::string_view inputName, std::string_view description, T &data)
    {
        if (findInput(inputName))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", inputName);

        auto param =
            inputs_.emplace(std::make_pair(inputName, std::make_shared<Parameter<T>>(this, inputName, description, data)))
                .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add bounded input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedInput(std::string_view inputName, std::string_view description, T &data,
                                                   std::optional<T> lower = {}, std::optional<T> upper = {},
                                                   std::optional<T> step = {})
    {
        if (findInput(inputName))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", inputName);

        auto param = inputs_
                         .emplace(std::make_pair(inputName, std::make_shared<BoundedParameter<T>>(this, inputName, description,
                                                                                                  data, lower, upper, step)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add bounded optional input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addBoundedOptionalInput(std::string_view inputName, std::string_view description, T &data,
                                                           T lower, std::string_view textWhenNull, T upper = {}, T step = {})
    {
        if (findInput(inputName))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", inputName);

        auto param =
            inputs_
                .emplace(std::make_pair(inputName, std::make_shared<BoundedOptionalParameter<T>>(
                                                       this, inputName, description, data, lower, textWhenNull, upper, step)))
                .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add output parameter
    template <class T>
    std::shared_ptr<ParameterBase> addOutput(std::string_view outputName, std::string_view description, T &data)
    {
        if (findOutput(outputName))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", outputName);

        auto param =
            outputs_.emplace(std::make_pair(outputName, std::make_shared<Parameter<T>>(this, outputName, description, data)))
                .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Add pointer output parameter
    template <typename ClassPtr>
    std::shared_ptr<ParameterBase> addPointerOutput(std::string_view outputName, std::string_view description,
                                                    std::remove_pointer<ClassPtr>::type &object)
    {
        if (findOutput(outputName))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", outputName);

        auto param = outputs_
                         .emplace(std::make_pair(
                             outputName, std::make_shared<PointerParameter<ClassPtr>>(this, outputName, description, object)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Add optional pointer output parameter
    template <typename ClassPtr>
    std::shared_ptr<ParameterBase> addOptionalPointerOutput(std::string_view outputName, std::string_view description,
                                                            std::optional<std::remove_pointer_t<ClassPtr>> &object)
    {
        if (findOutput(outputName))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", outputName);

        auto param = outputs_
                         .emplace(std::make_pair(outputName, std::make_shared<OptionalPointerParameter<ClassPtr>>(
                                                                 this, outputName, description, object)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Own supplied parameter
    bool ownParameter(std::shared_ptr<ParameterBase> &parameter, bool isOutput = false);
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findInput(std::string_view inputName) const;
    // Return input parameters
    NodeParameterMap &inputs();
    // Get input parameter value
    template <class T> T getInputValue(std::string_view inputName)
    {
        auto output = findInput(inputName);
        if (!output)
            Messenger::exception("Input '{}' does not exist.\n", inputName);

        // Get the upcast parameter
        auto upcast = output->upcast<T>();
        if (!upcast)
            Messenger::exception("Attempted to cast input '{}' to wrong type: is {}, requested {}.\n", inputName,
                                 output->type().name(), std::type_index(typeid(T)).name());

        // Return the parameter value
        return upcast->get();
    }
    // Return named output parameter if it exists
    std::shared_ptr<ParameterBase> findOutput(std::string_view outputName) const;
    // Return output parameters
    NodeParameterMap &outputs();
    // Get output parameter value
    template <class T> T getOutputValue(std::string_view outputName)
    {
        auto output = findOutput(outputName);
        if (!output)
            Messenger::exception("Output '{}' does not exist.\n", outputName);

        // Get the upcast parameter
        auto upcast = output->upcast<T>();
        if (!upcast)
            Messenger::exception("Attempted to cast output '{}' to wrong type: is {}, requested {}.\n", outputName,
                                 output->type().name(), std::type_index(typeid(T)).name());

        // Return the parameter value
        return upcast->get();
    }
    // Return named option if it exists
    std::shared_ptr<ParameterBase> findOption(std::string_view name) const;
    // Return options
    NodeParameterMap &options();
    // Get the incoming edges to this node
    EdgeMap &inputEdges();
    // Get the outgoing edges from this node
    EdgeMap &outputEdges();
    // Returns the node parent graph
    Graph *parentGraph() const;
    // Return the Dissolve reference
    virtual Dissolve &dissolve() const;

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
    // Is it appropriate to bother serialising this node?
    virtual bool shouldSerialise() const { return true; }
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
