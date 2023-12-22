// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "keywords/store.h"
#include "procedure/nodes/aliases.h"
#include "procedure/nodes/context.h"
#include "templates/optionalRef.h"

// Forward Declarations
class Configuration;
class CoreData;
class ExpressionVariable;
class GenericList;
class LineParser;
class ProcedureNodeSequence;
class ProcessPool;
class Site;

// Procedure Node
class ProcedureNode : public std::enable_shared_from_this<ProcedureNode>, public Serialisable<const CoreData &>
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
        AddPair,
        Box,
        CalculateAngle,
        CalculateAxisAngle,
        CalculateDistance,
        CalculateExpression,
        CalculateVector,
        Collect1D,
        Collect2D,
        Collect3D,
        CoordinateSets,
        Copy,
        CustomRegion,
        CylindricalRegion,
        DirectionalGlobalPotential,
        DynamicSite,
        GeneralRegion,
        Generate,
        ImportCoordinates,
        IntegerCollect1D,
        IfValueInRange,
        Integrate1D,
        IterateData1D,
        IterateSelection,
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
        PickProximity,
        PickRegion,
        Process1D,
        Process2D,
        Process3D,
        RegionalGlobalPotential,
        Remove,
        RestraintPotential,
        RotateFragment,
        RunLayer,
        Select,
        Sequence,
        SimpleGlobalPotential,
        SizeFactor,
        Sum1D,
        Temperature,
        Transmute
    };
    // Return enum option info for NodeType
    static EnumOptions<NodeType> nodeTypes();
    // Return the lowerCamelCase name of the node type provided
    static std::string lccNodeType(NodeType nodeType);
    // Node Contexts
    enum NodeContext
    {
        NoContext = 0,
        AnalysisContext = 1,
        GenerationContext = 2,
        OperateContext = 4,
        ControlContext = 8,
        AnyContext = 16,
        InheritContext = 32,
    };
    // Return enum option info for NodeContext
    static EnumOptions<NodeContext> nodeContexts();
    ProcedureNode(NodeType nodeType, std::vector<NodeContext> relevantContexts, NodeClass nodeClass = NodeClass::None);
    virtual ~ProcedureNode() = default;

    /*
     * Identity
     */
    protected:
    // Node type
    NodeType type_;
    // Relevant contexts for node
    std::vector<NodeContext> relevantContexts_;
    // Node class
    NodeClass class_;
    // Node name
    std::string name_;

    public:
    // Return node type
    NodeType type() const;
    // Return whether the supplied context is relevant for the current node
    bool isContextRelevant(NodeContext targetContext) const;
    // Return node class
    NodeClass nodeClass() const;
    // Return whether a name for the node must be provided
    virtual bool mustBeNamed() const;
    // Set node name
    void setName(std::string_view name);
    // Return node name
    std::string_view name() const;

    /*
     * Keywords
     */
    protected:
    // Keywords for this node
    KeywordStore keywords_;

    public:
    // Return keywords for this node
    KeywordStore &keywords();
    const KeywordStore &keywords() const;

    /*
     * Scope
     */
    private:
    // Scope (SequenceNode) in which this node exists
    OptionalReferenceWrapper<ProcedureNodeSequence> scope_;

    public:
    // Return the parent node which owns this node
    ProcedureNode *parent() const;
    // Set scope
    void setScope(ProcedureNodeSequence &scopeNode);
    // Return scope (ProcedureNodeSequence) in which this node exists
    OptionalReferenceWrapper<ProcedureNodeSequence> scope() const;
    // Return context of scope in which this node exists
    ProcedureNode::NodeContext scopeContext() const;
    // Return named node, optionally matching the type / class given, in or out of scope
    ConstNodeRef getNode(std::string_view name, bool onlyInScope, ConstNodeRef excludeNode = nullptr,
                         std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                         std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return nodes, optionally matching the type / class given, in or out of scope
    std::vector<ConstNodeRef> getNodes(bool onlyInScope, std::optional<ProcedureNode::NodeType> optNodeType = std::nullopt,
                                       std::optional<ProcedureNode::NodeClass> optNodeClass = std::nullopt) const;
    // Return the named parameter, in or out of scope
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name, bool onlyInScope,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter = nullptr) const;
    // Return all parameters in scope
    std::vector<std::shared_ptr<ExpressionVariable>> getParameters() const;

    /*
     * Branch
     */
    public:
    // Return the branch from this node (if it has one)
    virtual OptionalReferenceWrapper<ProcedureNodeSequence> branch();

    /*
     * Parameters
     */
    protected:
    // Defined parameters
    std::vector<std::shared_ptr<ExpressionVariable>> parameters_;
    // Set named parameter in supplied vector
    bool setParameter(std::vector<std::shared_ptr<ExpressionVariable>> &parameters, std::string_view parameter,
                      ExpressionValue value);

    public:
    // Add new parameter
    std::shared_ptr<ExpressionVariable> addParameter(std::string_view name, const ExpressionValue &initialValue = {});
    // Return the named parameter (if it exists)
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter = nullptr);
    // Return references to all parameters for this node
    const std::vector<std::shared_ptr<ExpressionVariable>> &parameters() const;

    /*
     * Execution
     */
    public:
    // Prepare any necessary data, ready for execution
    virtual bool prepare(const ProcedureContext &procedureContext);
    // Execute node
    virtual bool execute(const ProcedureContext &procedureContext);
    // Finalise any necessary data after execution
    virtual bool finalise(const ProcedureContext &procedureContext);

    /*
     * Read / Write
     */
    public:
    // Read node data from specified LineParser
    virtual bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write node data to specified LineParser
    virtual bool serialise(LineParser &parser, std::string_view prefix);
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
