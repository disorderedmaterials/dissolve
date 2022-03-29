// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "expression/expression.h"
#include "procedure/nodes/node.h"

// Forward Declarations
class Procedure;

// Sequence Node
class SequenceProcedureNode : public ProcedureNode
{
    public:
    SequenceProcedureNode(ProcedureNode::NodeContext context, const Procedure *procedure, NodeRef parentNode = nullptr,
                          std::string_view blockTerminationKeyword = "");
    ~SequenceProcedureNode() override;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Node Keywords
     */
    public:
    // Node Keywords
    enum SequenceNodeKeyword
    {
        nSequenceNodeKeywords
    };
    // Return enum option info for SequenceNodeKeyword
    static EnumOptions<SequenceNodeKeyword> sequenceNodeKeywords();

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

    public:
    // Clear all data
    void clear();
    // Add (own) node into sequence
    void addNode(NodeRef node);
    // Return sequential node list
    const std::vector<NodeRef> &sequence() const;
    // Return number of nodes in sequence
    int nNodes() const;

    /*
     * Scope
     */
    private:
    // Parent Procedure to which this sequence belongs
    const Procedure *procedure_;
    // Parent ProcedureNode in which this sequence exists
    NodeRef parentNode_;
    // Context of the sequence
    ProcedureNode::NodeContext context_;

    private:
    // Return named node if it exists anywhere in our sequence or below (and matches the type / class given)
    NodeRef searchNodes(std::string_view name, NodeRef excludeNode = nullptr,
                        std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                        std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Search through the Procedure for the named parameter
    std::shared_ptr<ExpressionVariable>
    searchParameters(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr) const;

    public:
    // Return parent Procedure to which this sequence belongs
    const Procedure *procedure() const override;
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
    ConstNodeRef nodeExists(std::string_view name, NodeRef excludeNode = nullptr,
                            std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                            std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return the named parameter if it is currently in scope
    std::shared_ptr<ExpressionVariable> parameterInScope(NodeRef queryingNode, std::string_view name,
                                                         const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr);
    // Return whether the named parameter exists in this sequence or its children (branches)
    std::shared_ptr<ExpressionVariable>
    parameterExists(std::string_view name, const std::shared_ptr<ExpressionVariable> &excludeParameter = nullptr) const;
    // Create and return reference list of parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> parametersInScope(ConstNodeRef queryingNode);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
    // Finalise any necessary data after execution
    bool finalise(const ProcedureContext &procedureContext) override;

    /*
     * Read / Write
     */
    private:
    // Block termination keyword for current context when reading
    std::string blockTerminationKeyword_;

    public:
    // Set block termination keyword for current context when reading
    void setBlockTerminationKeyword(std::string_view endKeyword);
    // Return block termination keyword for current context
    std::string_view blockTerminationKeyword() const;
    // Read structure from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData) override;
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix) override;
};
