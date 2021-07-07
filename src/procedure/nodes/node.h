// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "keywords/list.h"
#include "templates/listitem.h"
#include "templates/optionalref.h"

// Forward Declarations
class Configuration;
class CoreData;
class ExpressionVariable;
class GenericList;
class LineParser;
class NodeScopeStack;
class Procedure;
class ProcessPool;
class SequenceProcedureNode;
class Site;

// Procedure Node
class ProcedureNode : public ListItem<ProcedureNode>
{
    public:
    // Node Classes
    enum class NodeClass
    {
        None,
        Calculate,
        Operate,
        Pick,
        Region
    };
    // Return enum option info for NodeClass
    static EnumOptions<NodeClass> nodeClasses();
    // Node Types
    enum class NodeType
    {
        Add,
        Box,
        CalculateAngle,
        CalculateAxisAngle,
        CalculateDistance,
        CalculateVector,
        Collect1D,
        Collect2D,
        Collect3D,
        DynamicSite,
        Fit1D,
        GeneralRegion,
        Integrate1D,
        OperateDivide,
        OperateExpression,
        OperateGridNormalise,
        OperateMultiply,
        OperateNormalise,
        OperateNumberDensityNormalise,
        OperateSitePopulationNormalise,
        OperateSphericalShellNormalise,
        Parameters,
        Pick,
        PickCylinder,
        PickProximity,
        Process1D,
        Process2D,
        Process3D,
        Remove,
        Select,
        Sequence,
        Sum1D
    };
    // Return enum option info for NodeType
    static EnumOptions<NodeType> nodeTypes();
    // Node Contexts
    enum NodeContext
    {
        NoContext = 0,
        AnalysisContext = 1,
        GenerationContext = 2,
        OperateContext = 4
    };
    // Return enum option info for NodeContext
    static EnumOptions<NodeContext> nodeContexts();
    ProcedureNode(NodeType nodeType, NodeClass nodeClass = NodeClass::None);
    virtual ~ProcedureNode() = default;

    /*
     * Identity
     */
    protected:
    // Node class
    NodeClass class_;
    // Node type
    NodeType type_;
    // Node name
    std::string name_;
    // Node nice name
    std::string niceName_;

    public:
    // Return node class
    NodeClass nodeClass() const;
    // Return node type
    NodeType type() const;
    // Return whether specified context is relevant for this node type
    virtual bool isContextRelevant(NodeContext context) = 0;
    // Return whether a name for the node must be provided
    virtual bool mustBeNamed() const;
    // Set node name (and nice name)
    void setName(std::string_view name);
    // Return node name
    std::string_view name() const;
    // Return node nice name
    std::string_view niceName() const;

    /*
     * Keywords
     */
    protected:
    // Keywords for this node
    KeywordList keywords_;

    public:
    // Return keywords for this node
    const KeywordList &keywords() const;
    // Set specified keyword (pass-thru to KeywordList::set<D>())
    template <class D> bool setKeyword(std::string_view name, D value) { return keywords_.set<D>(name, value); }
    // Set specified enum keyword (pass-thru to KeywordList::setEnumeration<D>())
    template <class E> bool setEnumeration(std::string_view name, E enumeration)
    {
        return keywords_.setEnumeration<E>(name, enumeration);
    }

    /*
     * Scope
     */
    private:
    // Scope (SequenceNode) in which this node exists
    SequenceProcedureNode *scope_;

    public:
    // Set scope
    void setScope(SequenceProcedureNode *scopeNode);
    // Return scope (SequenceNode) in which this node exists
    SequenceProcedureNode *scope() const;
    // Return Procedure in which this node exists
    virtual const Procedure *procedure() const;
    // Return context of scope in which this node exists
    ProcedureNode::NodeContext scopeContext() const;
    // Return named node if it is currently in scope (and matches the type / class given)
    const ProcedureNode *nodeInScope(std::string_view name, const ProcedureNode *excludeNode = nullptr,
                                     std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                     std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return list of nodes in this node's scope (and matches the type / class given)
    std::vector<const ProcedureNode *> nodesInScope(std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                                    std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return named node if it exists anywhere in the same Procedure (and matches the type / class given)
    const ProcedureNode *nodeExists(std::string_view name, ProcedureNode *excludeNode = nullptr,
                                    std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                    std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return list of nodes (of specified type / class) present in the Procedure
    std::vector<const ProcedureNode *> nodes(std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                             std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return the named parameter if it is currently in scope
    std::shared_ptr<ExpressionVariable> parameterInScope(std::string_view name,
                                                         std::shared_ptr<ExpressionVariable> excludeParameter = nullptr);
    // Return the named parameter if it exists anywhere in the same Procedure
    std::shared_ptr<ExpressionVariable> parameterExists(std::string_view name,
                                                        std::shared_ptr<ExpressionVariable> excludeParameter = nullptr) const;
    // Create and return reference list of parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> parametersInScope();

    /*
     * Branch
     */
    public:
    // Return whether this node has a branch
    virtual bool hasBranch() const;
    // Return SequenceNode for the branch (if it exists)
    virtual SequenceProcedureNode *branch();

    /*
     * Parameters
     */
    public:
    // Return whether this node has the named parameter specified
    virtual std::shared_ptr<ExpressionVariable> hasParameter(std::string_view name,
                                                             std::shared_ptr<ExpressionVariable> excludeParameter = nullptr);
    // Return references to all parameters for this node
    virtual OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> parameters() const;

    /*
     * Execution
     */
    public:
    // Prepare any necessary data, ready for execution
    virtual bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    virtual bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Finalise any necessary data after execution
    virtual bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    /*
     * Read / Write
     */
    public:
    // Read node data from specified LineParser
    virtual bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write node data to specified LineParser
    virtual bool write(LineParser &parser, std::string_view prefix);
};
