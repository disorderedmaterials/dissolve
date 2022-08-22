// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "expression/expression.h"
#include "procedure/nodes/node.h"

// ProcedureNode Sequence
class ProcedureNodeSequence
{
    public:
    ProcedureNodeSequence(ProcedureNode::NodeContext context, OptionalReferenceWrapper<ProcedureNode> owner,
                          std::string_view blockKeyword);
    ~ProcedureNodeSequence();

    /*
     * Node Contents
     */
    protected:
    // Sequential node list
    std::vector<NodeRef> sequence_;
    // Range for query search
    class QueryRange
    {
        private:
        std::vector<NodeRef>::const_reverse_iterator start_, stop_;

        public:
        QueryRange(ConstNodeRef queryingNode, const std::vector<NodeRef> &seq);
        std::vector<NodeRef>::const_reverse_iterator begin();
        std::vector<NodeRef>::const_reverse_iterator end();
        bool empty();
        void next();
    };

    private:
    // Add (own) node into sequence
    void addNode(NodeRef nodeToAdd);

    public:
    // Clear all data
    void clear();
    // Create new node
    template <class N, typename... Args> std::shared_ptr<N> create(std::string_view name, Args &&... args)
    {
        // Create the new node
        auto node = std::make_shared<N>(args...);

        // Set its name
        node->setName(name);

        // Add node to our sequence, checking context / naming
        addNode(node);

        return node;
    }
    // Create new node by enumerated type
    std::shared_ptr<ProcedureNode> create(ProcedureNode::NodeType nodeType, std::string_view name);
    // Return sequential node list
    const std::vector<NodeRef> &sequence() const;
    // Return number of nodes in sequence
    int nNodes() const;
    // Return whether the sequence is empty
    bool empty() const;

    /*
     * Scope
     */
    private:
    // ProcedureNode which owns this sequence
    OptionalReferenceWrapper<ProcedureNode> owner_;
    // Context of the sequence
    ProcedureNode::NodeContext context_;

    private:
    // Return named node if it exists anywhere in our sequence or below (and matches the type / class given)
    NodeRef searchNodes(std::string_view name, ConstNodeRef excludeNode = nullptr,
                        std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                        std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Search through the Procedure for the named parameter
    std::shared_ptr<ExpressionVariable>
    searchParameters(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr) const;

    public:
    // Return this sequences owner
    OptionalReferenceWrapper<ProcedureNode> owner() const;
    // Return the context of the sequence
    ProcedureNode::NodeContext sequenceContext() const;
    // Return named node if present (and matches the type / class given)
    ConstNodeRef node(std::string_view name, std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                      std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return list of nodes (of specified type / class) present in the Procedure
    std::vector<ConstNodeRef> nodes(std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                    std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return named node if it is currently in scope (and matches the type / class given)
    ConstNodeRef nodeInScope(ConstNodeRef queryingNode, std::string_view name, ConstNodeRef excludeNode = nullptr,
                             std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                             std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return list of nodes in scope (and matching the type / class given)
    std::vector<ConstNodeRef> nodesInScope(ConstNodeRef queryingNode,
                                           std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                           std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return named node if it exists anywhere in the same Procedure (and matches the type / class given)
    ConstNodeRef nodeExists(std::string_view name, ConstNodeRef excludeNode = nullptr,
                            std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                            std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return the named parameter if it is currently in scope
    std::shared_ptr<ExpressionVariable> parameterInScope(ConstNodeRef queryingNode, std::string_view name,
                                                         const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr);
    // Return whether the named parameter exists in this sequence or its children (branches)
    std::shared_ptr<ExpressionVariable>
    parameterExists(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr) const;
    // Create and return reference list of parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> parametersInScope(ConstNodeRef queryingNode);
    // Check for node consistency
    bool check() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext);
    // Execute node
    bool execute(const ProcedureContext &procedureContext);
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext);

    /*
     * Read / Write
     */
    private:
    // Block keyword for current context when reading
    std::string blockKeyword_;

    public:
    // Return block keyword for current context
    std::string_view blockKeyword() const;
    // Read structure from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write structure to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix);
};
