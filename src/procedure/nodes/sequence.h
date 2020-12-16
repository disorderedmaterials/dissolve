// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/expression.h"
#include "procedure/nodes/node.h"
#include "templates/list.h"

// Forward Declarations
class Procedure;

// Sequence Node
class SequenceProcedureNode : public ProcedureNode
{
    public:
    SequenceProcedureNode(ProcedureNode::NodeContext context, const Procedure *procedure, ProcedureNode *parentNode = nullptr,
                          std::string_view blockTerminationKeyword = "");
    ~SequenceProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);

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
    List<ProcedureNode> sequence_;

    public:
    // Clear all data
    void clear();
    // Add (own) node into sequence
    void addNode(ProcedureNode *node);
    // Return sequential node list
    const List<ProcedureNode> &sequence() const;
    // Return number of nodes in sequence
    int nNodes() const;

    /*
     * Scope
     */
    private:
    // Parent Procedure to which this sequence belongs
    const Procedure *procedure_;
    // Parent ProcedureNode in which this sequence exists
    ProcedureNode *parentNode_;
    // Context of the sequence
    ProcedureNode::NodeContext context_;

    private:
    // Return named node if it exists anywhere in our sequence or below, and optionally matches the type given
    ProcedureNode *searchNodes(std::string_view name, ProcedureNode *excludeNode = nullptr,
                               ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
    // Search through the Procedure for the named parameter
    std::shared_ptr<ExpressionVariable> searchParameters(std::string_view name,
                                                         std::shared_ptr<ExpressionVariable> excludeParameter = nullptr) const;

    public:
    // Return parent Procedure to which this sequence belongs
    const Procedure *procedure() const;
    // Return the context of the sequence
    ProcedureNode::NodeContext sequenceContext() const;
    // Return named node if present, and which matches the (optional) type given
    ProcedureNode *node(std::string_view name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
    // Return list of nodes of specified type present in the Procedure
    RefList<ProcedureNode> nodes(ProcedureNode *queryingNode, ProcedureNode::NodeType nt);
    // Return named node if it is currently in scope, and optionally matches the type given
    ProcedureNode *nodeInScope(ProcedureNode *queryingNode, std::string_view name,
                               ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes);
    // Return list of nodes of specified type present in scope
    RefList<ProcedureNode> nodesInScope(ProcedureNode *queryingNode, ProcedureNode::NodeType nt);
    // Return named node if it exists anywhere in the same Procedure, and optionally matches the type given
    ProcedureNode *nodeExists(std::string_view name, ProcedureNode *excludeNode = nullptr,
                              ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
    // Return the named parameter if it is currently in scope
    std::shared_ptr<ExpressionVariable> parameterInScope(ProcedureNode *queryingNode, std::string_view name,
                                                         std::shared_ptr<ExpressionVariable> excludeParameter = nullptr);
    // Return whether the named parameter exists in this sequence or its children (branches)
    std::shared_ptr<ExpressionVariable> parameterExists(std::string_view name,
                                                        std::shared_ptr<ExpressionVariable> excludeParameter = nullptr) const;
    // Create and return reference list of parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> parametersInScope(ProcedureNode *queryingNode);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

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
    bool read(LineParser &parser, CoreData &coreData);
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
