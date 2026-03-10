// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/context.h"
#include "base/messenger.h"
#include "base/serialiser.h"
#include "module/module.h"
#include "nodes/constants.h"
#include "nodes/parameter.h"
#include "nodes/serialisableData.h"
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>

// Forward Declarations
class Graph;
class Edge;
class DissolveGraph;

// Node Base
class Node : public Serialisable<>
{
    public:
    Node() {}
    explicit Node(Graph *parentGraph) : parentGraph_(parentGraph) {}
    virtual ~Node() = default;

    /*
     * Definition
     */
    protected:
    // Node parent graph
    Graph *parentGraph_;

    public:
    // Position when presented graphically
    int x = 0, y = 0;
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
    // Get all nodes that lead into this node
    std::set<const Node *> allAncestors() const;
    // Print latest message
    void echo();

    public:
    // Get specific ancestors
    template <typename NodeKind> std::vector<const NodeKind *> ancestors() const
    {
        std::vector<const NodeKind *> result{};
        std::ranges::copy_if(allAncestors() | std::views::transform([](auto x) { return dynamic_cast<const NodeKind *>(x); }),
                             std::back_inserter(result), std::identity());
        return result;
    }
    // Message store vector
    const MessageStore &messages() const;
    // Returns true if message with given status exists
    bool hasMessages(MessageStatus status) const;

    /*
     * Processing & Validity
     */
    private:
    // Whether the node's data is up-to-date
    bool upToDate_{false};

    protected:
    // Version index for the node, bumped whenever result outputs change
    int versionIndex_{NodeConstants::InvalidVersion};
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
    virtual NodeConstants::ProcessResult run();

    /*
     * Inputs, Outputs, and Options
     */
    public:
    using NodeParameterMap = std::map<std::string, std::shared_ptr<ParameterBase>>;
    using EdgeMap = std::map<std::string_view, std::vector<Edge *>>;

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

    private:
    // Erase the specified edge from the given map, returning if it was found and erased
    bool eraseEdge(EdgeMap &map, Edge *edge);

    public:
    // Link edge, returning whether we accept it
    bool linkEdge(Edge *edge);
    // Unlink edge
    virtual void unlinkEdge(Edge *edge);
    // Add input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addOption(std::string_view optionName, std::string_view description, T &data)
    {
        if (findInput(optionName))
            Messenger::exception("Option '{}' already exists, and can't be added again.", optionName);

        auto param =
            options_
                .emplace(std::make_pair(optionName, ParameterFactory::createSerialisable(this, optionName, description, data)))
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

        auto param = inputs_.emplace(std::make_pair(inputName, ParameterFactory::create(this, inputName, description, data)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Input);
        return param;
    }
    // Add serialisable input parameter
    template <class T>
    std::shared_ptr<ParameterBase> addSerialisableInput(std::string_view inputName, std::string_view description, T &data)
    {
        if (findInput(inputName))
            Messenger::exception("Input parameter '{}' already exists, and can't be added again.", inputName);

        auto param =
            inputs_.emplace(std::make_pair(inputName, ParameterFactory::createSerialisable(this, inputName, description, data)))
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

        auto param = outputs_.emplace(std::make_pair(outputName, ParameterFactory::create(this, outputName, description, data)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Add pointer output parameter
    template <typename ClassObject>
    std::shared_ptr<ParameterBase> addPointerOutput(std::string_view outputName, std::string_view description,
                                                    ClassObject &object)
    {
        if (findOutput(outputName))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", outputName);

        auto param = outputs_
                         .emplace(std::make_pair(
                                                 outputName, ParameterFactory::createPointer<ClassObject>(this, outputName, description, object, {})))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Add optional pointer output parameter
    template <typename ClassObject>
    std::shared_ptr<ParameterBase> addOptionalPointerOutput(std::string_view outputName, std::string_view description,
                                                            std::optional<ClassObject> &optional)
    {
        if (findOutput(outputName))
            Messenger::exception("Output parameter '{}' already exists, and can't be added again.", outputName);

        auto param = outputs_
                         .emplace(std::make_pair(
                             outputName, ParameterFactory::createPointer<ClassObject>(this, outputName, description, optional)))
                         .first->second;
        param->setFlags(ParameterBase::ParameterFlags::Output);
        return param;
    }
    // Own supplied parameter
    bool ownParameter(std::shared_ptr<ParameterBase> &parameter, bool isOutput = false);
    // Return named input parameter if it exists
    std::shared_ptr<ParameterBase> findInput(std::string_view inputName) const;
    template <class T> bool setInput(std::string_view name, const T &value)
    {
        auto i = findInput(name);
        if (!i)
            return false;

        i->set<T>(value);
        return true;
    }
    // Return input parameters
    NodeParameterMap &inputs();
    // Get input parameter value
    template <class T> T getInputValue(std::string_view inputName)
    {
        auto output = findInput(inputName);
        if (!output)
            Messenger::exception("Input '{}' does not exist.\n", inputName);

        // Return the parameter value
        return output->get<T>();
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

        return output->get<T>();
    }
    // Return named option if it exists
    std::shared_ptr<ParameterBase> findOption(std::string_view name) const;
    // Return options
    NodeParameterMap &options();
    // Set option value
    template <class T> bool setOption(std::string_view name, const T &value)
    {
        auto opt = findOption(name);
        if (!opt)
            return false;

        opt->set<T>(value);
        return true;
    }
    // Get the incoming edges to this node
    EdgeMap &inputEdges();
    // Get the outgoing edges from this node
    virtual EdgeMap &outputEdges();
    // Mark incoming edges to the specified parameter as needing a re-pull
    void markIncomingEdgesForPull(const ParameterBase *toParameter) const;
    // Returns the node parent graph
    Graph *parentGraph() const;
    // Return the Dissolve reference
    virtual Dissolve &dissolve() const;
    // Return the DissolveGraph reference
    virtual DissolveGraph *dissolveGraph();

    /*
     * Data
     */
    private:
    // Accumulated timing information (in seconds)
    SampledDouble timing_;

    public:
    // Clear any local data
    virtual void clearData();
    // Return timing information (in seconds)
    SampledDouble timing() const;

    /*
     * Serialisation
     */
    protected:
    // Persistent data serialisables
    std::map<std::string, std::shared_ptr<SerialisableData>> serialisables_;
    // Serialise any hidden content
    virtual void serialiseInternal(SerialisedValue &target) const {}
    // Deserialise any hidden content
    virtual void deserialiseInternal(const SerialisedValue &node) {}

    public:
    // Update the parent graph after a move
    void setParent(Graph *graph);
    // Is it appropriate to bother serialising this node?
    virtual bool shouldSerialise() const { return true; }
    // Flag a persistent serialisable quantity
    template <typename DataClass> void addSerialisable(std::string_view key, DataClass &data)
    {
        serialisables_[std::string(key)] = std::make_shared<SerialisableClass<DataClass>>(key, data);
    }
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    // Express persistent data as a serialisable value
    SerialisedValue serialiseData() const;
    // Read persistent data from a serialisable value
    void deserialiseData(const SerialisedValue &node);
};
