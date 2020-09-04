/*
    *** Procedure Node
    *** src/procedure/nodes/node.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/enumoptions.h"
#include "keywords/list.h"
#include "templates/listitem.h"

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
    // Node Types
    enum NodeType
    {
        AddSpeciesNode,
        BoxNode,
        BEGIN_CalculateNodes,
        CalculateAngleNode,
        CalculateAxisAngleNode,
        CalculateDistanceNode,
        CalculateBaseNode,
        CalculateVectorNode,
        Collect1DNode,
        Collect2DNode,
        Collect3DNode,
        END_CalculateNodes,
        DynamicSiteNode,
        ExcludeNode,
        Fit1DNode,
        Integrate1DNode,
        BEGIN_OperateNodes,
        OperateBaseNode,
        OperateDivideNode,
        OperateExpressionNode,
        OperateGridNormaliseNode,
        OperateMultiplyNode,
        OperateNormaliseNode,
        OperateNumberDensityNormaliseNode,
        OperateSitePopulationNormaliseNode,
        OperateSphericalShellNormaliseNode,
        END_OperateNodes,
        ParametersNode,
        Process1DNode,
        Process2DNode,
        Process3DNode,
        SelectNode,
        SequenceNode,
        Sum1DNode,
        nNodeTypes
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
    ProcedureNode(NodeType nodeType);
    virtual ~ProcedureNode();

    /*
     * Identity
     */
    protected:
    // Node type
    NodeType type_;
    // Node name
    std::string name_;
    // Node nice name
    std::string niceName_;

    public:
    // Return node type
    NodeType type() const;
    // Return whether the node is of the specified type (detecting derived node classes as well)
    bool isType(NodeType thisType) const;
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
    const Procedure *procedure() const;
    // Return context of scope in which this node exists
    ProcedureNode::NodeContext scopeContext() const;
    // Return named node if it is currently in scope, and optionally matches the type given
    ProcedureNode *nodeInScope(std::string_view name, ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes);
    // Return list of nodes of specified type present in this node's scope
    RefList<ProcedureNode> nodesInScope(ProcedureNode::NodeType nt);
    // Return named node if it exists anywhere in the same Procedure, and optionally matches the type given
    ProcedureNode *nodeExists(std::string_view name, ProcedureNode *excludeNode = nullptr,
                              ProcedureNode::NodeType nt = ProcedureNode::nNodeTypes) const;
    // Return list of nodes of specified type present in the Procedure
    RefList<ProcedureNode> nodes(ProcedureNode::NodeType nt);
    // Return whether the named parameter is currently in scope
    ExpressionVariable *parameterInScope(std::string_view name, ExpressionVariable *excludeParameter = nullptr);
    // Return whether the named parameter exists anywhere in the same Procedure
    ExpressionVariable *parameterExists(std::string_view name, ExpressionVariable *excludeParameter = nullptr) const;
    // Create and return reference list of parameters in scope
    RefList<ExpressionVariable> parametersInScope();

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
    virtual ExpressionVariable *hasParameter(std::string_view name, ExpressionVariable *excludeParameter = nullptr);
    // Return references to all parameters for this node
    virtual RefList<ExpressionVariable> parameterReferences() const;

    /*
     * Execution
     */
    public:
    // Node execution result
    enum NodeExecutionResult
    {
        Failure,
        Success,
        SomethingElse
    };
    // Prepare any necessary data, ready for execution
    virtual bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    virtual NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                        GenericList &targetList) = 0;
    // Finalise any necessary data after execution
    virtual bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    /*
     * Read / Write
     */
    public:
    // Read node data from specified LineParser
    virtual bool read(LineParser &parser, CoreData &coreData);
    // Write node data to specified LineParser
    virtual bool write(LineParser &parser, std::string_view prefix);
};
